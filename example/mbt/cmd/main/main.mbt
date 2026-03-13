///|
using @lib {
  type QApplication,
  type QWidget,
  type QHBoxLayout,
  type QPushButton,
  type QLabel,
}

///|
fn main {
  let app = QApplication::new(@env.args())

  let mut count = 0

  let label = QLabel::new()
  label.setText("Count: 0")

  let button = QPushButton::new()
  button.setText("Click me!")
  button.clicked(fn() {
    count += 1
    label.setText("Count: \{count}")
  })

  let layout = QHBoxLayout::new()
  layout.addWidget(button.as_QWidget())
  layout.addWidget(label.as_QWidget())

  let window = QWidget::new()
  window.setWindowTitle("Counter")
  window.setLayout(layout.as_QLayout())
  window.show()

  let _ = app.exec()
}
