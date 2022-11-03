qstyleshaker
============

### Overview

I like using Qt to develop desktop applications, but I also like applications
that I use to have dark mode. I could write a custom stylesheet for each of my
applications, but how do I make sure they're consistent?

I could use a stylesheet like QBreezeStyle, but what if my application is only a
few widgets? I want their stylesheets to be as minimal as possible.

Enter `qstyleshaker`, a very basic tool for tree shaking Qt css files.

### Usage

```
Usage: ./qstyleshaker [options]
Tree shaker for Qt Stylesheets

Options:
  -h, --help                     Displays help on commandline options.
  --help-all                     Displays help including Qt specific options.
  -d, --directory <directory>    Use source files in <directory>.
  -f, --footer <footer>          Print <footer>'s content at the bottom of
                                 output.
  -t, --top <header>             Print <header>'s content at the top of output.
  -s, --stylesheet <stylesheet>  Use <stylesheet> as a source.
```

### What it does

This tool works by analyzing the Qt header includes found in `.cpp` and `.h`
files. It does not perform any analysis of your code. If, for example, your
code includes `QTreeWidget`, then all `QTreeView` rules are kept.

Aside from removing rules for classes not included, the tool provides simple
merging for adjacent rules:

```
QComboBox               |     QComboBox,
{                       |     QPushButton
    color: black;       |     {
}                       |         color: black;
QPushButton             |     }
{                       |
    color: black;       |
}                       |
```

```
QComboBox               |    QComboBox
{                       |    {
    color: white;       |        background: black;
}                       |        color: white;
QComboBox               |    }
{                       |
    background: black;  |
}                       |
```

### Caveats

The tool is focused on not altering the resulting output, so non-adjacent rules
are not merged together.

The tool is not recognize custom classes or selectors that work by identity.

### Why

I want my Qt projects to have a consistent style, but I don't want them to have
stylesheet rules they don't use.
