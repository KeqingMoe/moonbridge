///|
pub trait Object {
  as_object(Self) -> &Object
}

///|
pub type QObject

///|
pub impl Object for QObject with as_object(self) {
  self
}

///|
pub type QWidget

///|
pub extern "C" fn QWidget::new() -> QWidget = "QWidget_new"

///|
#borrow(self)
pub extern "C" fn QWidget::show(self : Self) = "QWidget_show"

///|
#borrow(self, title)
pub extern "C" fn QWidget::setWindowTitle(self : Self, title : String) = "QWidget_setWindowTitle"

///|
#borrow(self, layout)
pub extern "C" fn QWidget::setLayout(self : Self, layout : QLayout) = "QWidget_setLayout"

///|
#borrow(self)
pub extern "C" fn QWidget::as_QObject(self : Self) -> QObject = "QWidget_as_QObject"

///|
pub impl Object for QWidget with as_object(self) {
  self.as_QObject()
}

///|
pub type QApplication

///|
#borrow(args)
pub extern "C" fn QApplication::new(args : Array[String]) -> QApplication = "QApplication_new"

///|
#borrow(self)
pub extern "C" fn QApplication::exec(self : Self) -> Int = "QApplication_exec"

///|
#borrow(self)
pub extern "C" fn QApplication::as_QObject(self : Self) -> QObject = "QApplication_as_QObject"

///|
pub impl Object for QApplication with as_object(self) {
  self.as_QObject()
}

///|
pub type QLayout

///|
pub type QHBoxLayout

///|
pub extern "C" fn QHBoxLayout::new() -> Self = "QHBoxLayout_new"

///|
#borrow(self, widget)
pub extern "C" fn QHBoxLayout::addWidget(self : Self, widget : QWidget) = "QHBoxLayout_addWidget"

///|
#borrow(self)
pub extern "C" fn QHBoxLayout::as_QLayout(self : Self) -> QLayout = "QHBoxLayout_as_QLayout"

///|
pub type QLabel

///|
pub extern "C" fn QLabel::new() -> Self = "QLabel_new"

///|
#borrow(self, text)
pub extern "C" fn QLabel::setText(self : Self, text : String) = "QLabel_setText"

///|
#borrow(self)
pub extern "C" fn QLabel::as_QWidget(self : Self) -> QWidget = "QLabel_as_QWidget"

///|
pub type QPushButton

///|
pub extern "C" fn QPushButton::new() -> Self = "QPushButton_new"

///|
#borrow(self, text)
pub extern "C" fn QPushButton::setText(self : Self, text : String) = "QPushButton_setText"

///|
#borrow(self)
#owned(cb)
pub extern "C" fn QPushButton::clicked(self : Self, cb : () -> Unit) = "QPushButton_clicked"

///|
#borrow(self)
pub extern "C" fn QPushButton::as_QWidget(self : Self) -> QWidget = "QPushButton_as_QWidget"

///|
#borrow(cb)
pub extern "C" fn test_cb(cb : () -> Unit) = "test_cb"
