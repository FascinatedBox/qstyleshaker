#ifndef PARENT_TABLE_H
# define PARENT_TABLE_H

QHash<QString, QStringList> parent_table;

void init_parent_table(void)
{
    QHash<QString, QStringList> h;

    h["QAbstractButton"] = QStringList("QWidget");
    h["QAbstractItemView"] = QStringList("QAbstractScrollArea");
    h["QAbstractScrollArea"] = QStringList("QFrame") << "QScrollBar";
    h["QAbstractSlider"] = QStringList("QWidget");
    h["QAbstractSpinBox"] = QStringList("QWidget");
    h["QCalendarWidget"] = QStringList("QWidget");
    h["QCheckBox"] = QStringList("QAbstractButton");
    h["QColorDialog"] = QStringList("QDialog");
    h["QColumnView"] = QStringList("QAbstractItemView");
    h["QComboBox"] = QStringList("QWidget");
    h["QCommandLinkButton"] = QStringList("QPushButton");
    h["QDateEdit"] = QStringList("QDateTimeEdit");
    h["QDateTimeEdit"] = QStringList("QAbstractSpinBox");
    h["QDesktopWidget"] = QStringList("QWidget");
    h["QDial"] = QStringList("QAbstractSlider");
    h["QDialog"] = QStringList("QWidget");
    h["QDialogButtonBox"] = QStringList("QWidget") << "QPushButton";
    h["QDockWidget"] = QStringList("QWidget");
    h["QDoubleSpinBox"] = QStringList("QAbstractSpinBox");
    h["QErrorMessage"] = QStringList("QDialog");
    h["QFileDialog"] = QStringList("QDialog");
    h["QFocusFrame"] = QStringList("QWidget");
    h["QFontComboBox"] = QStringList("QComboBox");
    h["QFontDialog"] = QStringList("QDialog");
    h["QFrame"] = QStringList("QWidget");
    h["QGraphicsView"] = QStringList("QAbstractScrollArea");
    h["QGroupBox"] = QStringList("QWidget");
    h["QHeaderView"] = QStringList("QAbstractItemView");
    h["QInputDialog"] = QStringList("QDialog");
    h["QKeySequenceEdit"] = QStringList("QWidget");
    h["QLCDNumber"] = QStringList("QFrame");
    h["QLabel"] = QStringList("QFrame");
    h["QLineEdit"] = QStringList("QWidget");
    h["QListView"] = QStringList("QAbstractItemView");
    h["QListWidget"] = QStringList("QListView");
    h["QMacCocoaViewContainer"] = QStringList("QWidget");
    h["QMacNativeWidget"] = QStringList("QWidget");
    h["QMainWindow"] = QStringList("QWidget");
    h["QMdiArea"] = QStringList("QAbstractScrollArea");
    h["QMdiSubWindow"] = QStringList("QWidget");
    h["QMenu"] = QStringList("QWidget");
    h["QMenuBar"] = QStringList("QWidget");
    h["QMessageBox"] = QStringList("QDialog");
    h["QOpenGLWidget"] = QStringList("QWidget");
    h["QPlainTextEdit"] = QStringList("QAbstractScrollArea");
    h["QProgressBar"] = QStringList("QWidget");
    h["QProgressDialog"] = QStringList("QDialog");
    h["QPushButton"] = QStringList("QAbstractButton");
    h["QRadioButton"] = QStringList("QAbstractButton");
    h["QRubberBand"] = QStringList("QWidget");
    h["QScrollArea"] = QStringList("QAbstractScrollArea");
    h["QScrollBar"] = QStringList("QAbstractSlider");
    h["QSizeGrip"] = QStringList("QWidget");
    h["QSlider"] = QStringList("QAbstractSlider");
    h["QSpinBox"] = QStringList("QAbstractSpinBox");
    h["QSplashScreen"] = QStringList("QWidget");
    h["QSplitter"] = QStringList("QFrame");
    h["QSplitterHandle"] = QStringList("QWidget");
    h["QStackedWidget"] = QStringList("QFrame");
    h["QStatusBar"] = QStringList("QWidget");
    h["QTabBar"] = QStringList("QWidget");
    h["QTabWidget"] = QStringList("QWidget") << "QTabBar";
    h["QTableView"] = QStringList("QAbstractItemView");
    h["QTableWidget"] = QStringList("QTableView");
    h["QTextBrowser"] = QStringList("QTextEdit");
    h["QTextEdit"] = QStringList("QAbstractScrollArea");
    h["QTimeEdit"] = QStringList("QDateTimeEdit");
    h["QToolBar"] = QStringList("QWidget");
    h["QToolBox"] = QStringList("QFrame");
    h["QToolButton"] = QStringList("QAbstractButton");
    h["QTreeView"] = QStringList("QAbstractItemView");
    h["QTreeWidget"] = QStringList("QTreeView");
    h["QUndoView"] = QStringList("QListView");
    h["QWizard"] = QStringList("QDialog");
    h["QWizardPage"] = QStringList("QWidget");

    parent_table = h;
}

#endif
