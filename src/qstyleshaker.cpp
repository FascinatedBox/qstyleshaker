#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDirIterator>
#include <QRegularExpression>
#include <QSet>
#include <QTextStream>

#include "parent_table.h"

class Block
{
public:
    Block() {};

    // Selector lines
    QList<QString> selector_lines;

    // For each selector, what classes are used
    QList<QList<QString>> all_selector_classes;

    // actions inside of the block
    QList<QString> action_lines;
};

class Options {
public:
    Options() {};

    QList<QString> directories;
    QString footer;
    QString header;
    QString prefix;
    QString stylesheet;
};

Options *parseOptions(QCoreApplication *app, int _argc, char **_argv)
{
    QCoreApplication::setApplicationName("qstyleshaker");
    QCoreApplication::setApplicationVersion(".1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Tree shaker for Qt Stylesheets");
    parser.addHelpOption();

    QCommandLineOption directoryOption(QStringList() << "d" << "directory",
            QCoreApplication::translate("main", "Use source files in <directory>."),
            QCoreApplication::translate("main", "directory"));
    parser.addOption(directoryOption);

    QCommandLineOption footerOption(QStringList() << "f" << "footer",
            QCoreApplication::translate("main", "Print <footer>'s content at the bottom of output."),
            QCoreApplication::translate("main", "footer"));
    parser.addOption(footerOption);

    QCommandLineOption prefixOption(QStringList() << "p" << "prefix",
            QCoreApplication::translate("main", "Add <prefix> to *all* selectors."),
            QCoreApplication::translate("main", "prefix"));
    parser.addOption(prefixOption);

    QCommandLineOption headerOption(QStringList() << "t" << "top",
            QCoreApplication::translate("main", "Print <header>'s content at the top of output."),
            QCoreApplication::translate("main", "header"));
    parser.addOption(headerOption);

    QCommandLineOption stylesheetOption(QStringList() << "s" << "stylesheet",
            QCoreApplication::translate("main", "Use <stylesheet> as a source."),
            QCoreApplication::translate("main", "stylesheet"));
    parser.addOption(stylesheetOption);

    parser.process(*app);

    Options *result = new Options;

    result->directories = parser.values(directoryOption);
    result->prefix = parser.value(prefixOption);
    result->stylesheet = parser.value(stylesheetOption);
    result->header = parser.value(headerOption);
    result->footer = parser.value(footerOption);

    if (result->directories.size() == 0) {
        qCritical("qstyleshaker: No directories (-d, --directory) given. Stopping.");
        ::exit(EXIT_FAILURE);
    }

    if (result->stylesheet == "") {
        qCritical("qstyleshaker: No stylesheet (-s, --stylesheet) given. Stopping.");
        ::exit(EXIT_FAILURE);
    }

    return result;
}

QList<QString> read_dir_for_targets(QString basedir)
{
    QDirIterator it(basedir);
    QList<QString> result;

    while (it.hasNext()) {
        it.next();

        QString path = it.filePath();

        if (path.endsWith(".cpp") == false &&
            path.endsWith(".h") == false)
            continue;

        result.append(path);
    }

    return result;
}


QList<QString> read_all_dirs(QList<QString> dirlist)
{
    QList<QString> result;

    foreach (QString dir, dirlist)
        result.append(read_dir_for_targets(dir));

    return result;
}

QSet<QString> get_styleable_classes_used(QList<QString> path_list)
{
    QSet<QString> class_set;
    QRegularExpression rx("\\s*#\\s*include\\s*<(Q\\w+)>\\s*");

    foreach (QString path, path_list) {
        QFile f(path);

        f.open(QIODevice::ReadOnly | QIODevice::Text);

        QTextStream in(&f);

        while (in.atEnd() == false) {
            QString line = in.readLine();

            if (line.indexOf("#") == -1)
                continue;

            auto match = rx.match(line);

            if (match.hasMatch() == false)
                continue;

            class_set.insert(match.captured(1));
        }
    }

    // class_set is all classes directly referenced.
    // For each class directly referenced, go through the parent table and find
    // out what visible classes to reference.
    QSet<QString> result;
    QSet<QString>::const_iterator i = class_set.constBegin();
    QStringList walk_list;
    QStringList empty;

    while (1) {
        QString key;

        if (walk_list.size() == 0) {
            if (i == class_set.constEnd())
                break;
            else {
                key = *i;
                i++;
            }
        }
        else
            key = walk_list.takeAt(0);

        QStringList parent_list = parent_table.value(key, empty);

        if (parent_list.size()) {
            result.insert(key);

            foreach (QString p, parent_list)
                result.insert(p);

            walk_list += parent_list;
        }
    }

    return result;
}

QString decomment_stylesheet(QString stylesheet_text)
{
    QRegularExpression rx("/\\*(.*?)\\*/",
            QRegularExpression::DotMatchesEverythingOption);
    QString result = stylesheet_text.replace(rx, "");
    return result;
}

QString stylesheet_to_string(QString path)
{
    QFile f(path);
    QString result;

    if (false == f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("qstyleshaker: Can't open stylesheet at '%s'.",
                qUtf8Printable(path));
        ::exit(EXIT_FAILURE);
        return result;
    }

    result = QString(f.readAll());
    f.close();
    return result;
}

QList<Block *> stylesheet_to_blocks(QString stylesheet_text)
{
    QStringList lines = stylesheet_text.split("\n");
    bool in_block = false;
    auto selector_lines = QList<QString>();
    auto all_selector_classes = QList<QList<QString>>();
    auto action_lines = QList<QString>();
    QRegularExpression rx("(Q\\w+)");
    QList<Block *> result;

    foreach (QString line, lines) {
        char ch;
        int i;

        for (i = 0;i < line.size();i++) {
            ch = line[i].toLatin1();

            if (ch != ' ' && ch != '\t')
                break;
        }

        if (i == line.size())
            continue;

        if (in_block == false) {
            if (isalnum(ch)) {
                auto it = rx.globalMatch(line);
                QList<QString> captures;

                while (it.hasNext())
                    captures.append(it.next().captured(1));

                all_selector_classes.append(captures);
                line = line.trimmed();

                if (line.endsWith(","))
                    line.chop(1);

                selector_lines.append(line);
            }
            else if (ch == '{')
                in_block = true;
        }
        else {
            if (ch != '}')
                action_lines.append(line);
            else {
                Block *b = new Block;

                in_block = false;
                b->selector_lines = selector_lines;
                b->all_selector_classes = all_selector_classes;
                b->action_lines = action_lines;
                selector_lines = QList<QString>();
                all_selector_classes = QList<QList<QString>>();
                action_lines = QList<QString>();
                result.append(b);
            }
        }
    }

    return result;
}

void drop_unused_selectors(Block *b, QSet<QString> classes_used)
{
    auto selector_lines = b->selector_lines;
    auto all_selector_classes = b->all_selector_classes;

    for (int i = 0;i < all_selector_classes.size();i++) {
        auto class_list = all_selector_classes[i];
        bool ok = true;

        foreach (QString c, class_list) {
            if (classes_used.contains(c) == false) {
                ok = false;
                break;
            }
        }

        if (ok == true)
            continue;

        all_selector_classes.removeAt(i);
        selector_lines.removeAt(i);
        i--;
    }

    b->selector_lines = selector_lines;
    b->all_selector_classes = all_selector_classes;
}

void drop_url_lines(Block *b)
{
    auto action_lines = b->action_lines;

    for (int i = 0;i < action_lines.size();i++) {
        QString a = action_lines[i];
        if (a.contains("url(:/")) {
            action_lines.removeAt(i);
            i--;
        }
    }

    b->action_lines = action_lines;
}

bool should_keep_block(Block *b)
{
    bool result = true;

    if (b->action_lines.size() == 0)
        result = false;

    if (b->selector_lines.size() == 0)
        result = false;

    return result;
}

void maybe_fix_selectors(Block *b)
{
    for (int i = 0;i < b->selector_lines.size();i++) {
        QString line = b->selector_lines[i];

        // This particular selector will not work unless there is an empty
        // comment after it. No idea why, but work around it until it's fixed.
        if (line.startsWith("QFrame") &&
            line == "QFrame[frameShape=\"6\"]")
            b->selector_lines[i] = line + "/**/";
    }
}

void sort_block_actions(Block *b)
{
    bool can_sort = true;

    foreach (QString line, b->action_lines) {
        // Simple check for an action that spans multiple lines. Simple sorting
        // will almost certainly result in broken css, so don't do that.
        if (line.endsWith(";") == false) {
            can_sort = false;
            break;
        }

        // Could have 'padding-top' and 'padding' or 'border-left' and
        // 'border'. Sorting might change the result, so don't do that.
        if (line.contains("-bottom") == true ||
            line.contains("-left") == true ||
            line.contains("-right") == true ||
            line.contains("-top") == true) {
            can_sort = false;
            break;
        }
    }

    if (can_sort == false)
        return;

    std::sort(b->action_lines.begin(), b->action_lines.end());
}

bool block_compare(Block *a, Block *b)
{
    return a->selector_lines < b->selector_lines;
}

// Merge blocks that are adjacent to each other, since that will not change the
// resulting order of rules.
QList<Block *> merge_adjacent_blocks(QList<Block *> blocks)
{
    QList<Block *> result;

    for (int i = 0;i < blocks.size();i++) {
        Block *left = blocks[i];

        if (left->selector_lines.size() == 0)
            continue;

        if (i == blocks.size() - 1) {
            result.append(left);
            break;
        }

        Block *right = blocks[i + 1];

        if (left->selector_lines.size() == right->selector_lines.size() &&
            left->selector_lines == right->selector_lines) {
            // Same selector, different actions:
            //
            // QTreeView::item
            // {
            //     ....
            // }
            //
            // QTreeView::item
            // {
            //     ....
            // }
            //
            // Take the actions, leave the selector. This should be sufficient.
            left->action_lines += right->action_lines;
            right->selector_lines = QList<QString>();
        }
        else if (left->action_lines.size() == right->action_lines.size() &&
                 left->action_lines == right->action_lines) {
            // Different selectors, same actions:
            //
            // QTreeView::item
            // {
            //     color: red;
            // }
            //
            // QPushButton
            // {
            //     color: red;
            // }
            //
            // Take the selectors, leave the actions. This runs after the
            // initial block shake, so all_selector_classes does not need to be
            // adjusted.
            left->selector_lines += right->selector_lines;
            right->selector_lines = QList<QString>();
        }

        result.append(left);
    }

    return result;
}

QList<Block *> shake_blocks(QList<Block *> blocks, QSet<QString> classes_used)
{
    QList<Block *> filtered_blocks;

    foreach (Block *b, blocks) {
        drop_unused_selectors(b, classes_used);
        drop_url_lines(b);

        if (should_keep_block(b))
            filtered_blocks.append(b);

        sort_block_actions(b);
        maybe_fix_selectors(b);
    }

    filtered_blocks = merge_adjacent_blocks(filtered_blocks);
    return filtered_blocks;
}

void print_file(QString path)
{
    if (path == "")
        return;

    QFile f(path);

    if (false == f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical("qstyleshaker: Can't open header at '%s'.",
                qUtf8Printable(path));
        ::exit(EXIT_FAILURE);
    }

    QString s = QString(f.readAll());

    fputs(qPrintable(s), stdout);
    f.close();
}

void apply_prefix_to_block_selectors(Options *o, QList<Block *> blocks)
{
    if (o->prefix == "")
        return;

    QString prefix = o->prefix + " ";

    foreach (Block *b, blocks)
        for (int i = 0;i < b->selector_lines.size();i++)
            b->selector_lines[i].prepend(prefix);
}

void print_blocks(QList<Block *> blocks)
{
    bool first = true;

    foreach (Block *b, blocks) {
        if (first)
            first = false;
        else
            fputs("\n", stdout);

        QString selector_lines = b->selector_lines.join(",\n");

        fputs(qPrintable(selector_lines), stdout);
        fputs("\n{\n", stdout);

        for (int i = 0;i < b->action_lines.size();i++) {
            QString s = b->action_lines[i];

            fputs(qPrintable(s), stdout);
            fputs("\n", stdout);
        }

        fputs("}\n", stdout);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    init_parent_table();

    auto options = parseOptions(&app, argc, argv);
    auto path_list = read_all_dirs(options->directories);
    auto class_set = get_styleable_classes_used(path_list);
    auto stylesheet_text = stylesheet_to_string(options->stylesheet);
    stylesheet_text = decomment_stylesheet(stylesheet_text);
    auto stylesheet_blocks = stylesheet_to_blocks(stylesheet_text);
    stylesheet_blocks = shake_blocks(stylesheet_blocks, class_set);
    apply_prefix_to_block_selectors(options, stylesheet_blocks);
    print_file(options->header);
    print_blocks(stylesheet_blocks);
    print_file(options->footer);
    return EXIT_SUCCESS;
}
