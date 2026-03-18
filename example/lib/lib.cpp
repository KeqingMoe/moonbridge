#include <moonbridge.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPointer>
#include <QWidget>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#pragma clang diagnostic ignored "-Wunknown-pragmas" // 不加这行会导致 pop 报警告，我也不知道为什么

namespace qt
{

using QObject      = QPointer<QObject>;
using QWidget      = QPointer<QWidget>;
using QLayout      = QPointer<QLayout>;
using QApplication = QPointer<QApplication>;
using QHBoxLayout  = QPointer<QHBoxLayout>;
using QLabel       = QPointer<QLabel>;
using QPushButton  = QPointer<QPushButton>;

}

extern "C"
{
    using namespace mbt::types;
    using namespace mbt::ffi;

    auto QApplication_new(Array<String> args) -> box<qt::QApplication>
    {
        // 只能创建一个 Application 对象
        // argc 和 argv 必须可变全程存活
        // 受制于 Qt 的神秘设计，不得不如此

        static struct
        {
            int argc = 0;
            std::vector<std::string> argarr;
            std::vector<char*> argv;
        } Args;

        if (Args.argc != 0 || args.size() == 0) {
            panic();
        }

        Args.argc = args.size();
        for (auto arg : args) {
            auto u16s = QString::fromUtf16(arg.data(), arg.size());
            Args.argarr.push_back(u16s.toStdString());
        }
        for (auto&& arg : Args.argarr) {
            Args.argv.push_back(arg.data());
        }
        Args.argv.push_back(nullptr);

        auto&& argc = Args.argc;
        auto&& argv = Args.argv.data();
        return box<qt::QApplication>::make(new QApplication(argc, argv));
    }

    auto QApplication_exec(box<qt::QApplication> self) -> Int
    {
        return (*self)->exec();
    }

    auto QApplication_as_QObject(box<qt::QApplication> self) -> box<qt::QObject>
    {
        return box<qt::QObject>::make(qobject_cast<QObject*>(self.repr->get()));
    }

    auto QWidget_new() -> box<qt::QWidget>
    {
        return box<qt::QWidget>::make(new QWidget());
    }

    auto QWidget_show(box<qt::QWidget> self) -> void
    {
        (*self)->show();
    }

    auto QWidget_setWindowTitle(box<qt::QWidget> self, String title) -> void
    {
        (*self)->setWindowTitle(QString::fromUtf16(title.data(), title.size()));
    }

    auto QWidget_as_QObject(box<qt::QWidget> self) -> box<qt::QObject>
    {
        return box<qt::QObject>::make(qobject_cast<QObject*>(self.repr->get()));
    }

    auto QWidget_setLayout(box<qt::QWidget> self, box<qt::QLayout> layout)
    {
        (*self)->setLayout(layout->get());
    }

    auto QHBoxLayout_new() -> box<qt::QHBoxLayout>
    {
        return box<qt::QHBoxLayout>::make(new QHBoxLayout());
    }

    auto QHBoxLayout_addWidget(box<qt::QHBoxLayout> self, box<qt::QWidget> widget) -> void
    {
        (*self)->addWidget(widget->get());
    }

    auto QHBoxLayout_as_QLayout(box<qt::QHBoxLayout> self) -> box<qt::QLayout>
    {
        return box<qt::QLayout>::make(qobject_cast<QLayout*>(self.repr->get()));
    }

    auto QLabel_new() -> box<qt::QLabel>
    {
        return box<qt::QLabel>::make(new QLabel());
    }

    auto QLabel_setText(box<qt::QLabel> self, String text) -> void
    {
        (*self)->setText(QString::fromUtf16(text.data(), text.size()));
    }

    auto QLabel_as_QWidget(box<qt::QLabel> self) -> box<qt::QWidget>
    {
        return box<qt::QWidget>::make(qobject_cast<QWidget*>(self.repr->get()));
    }

    auto QPushButton_new() -> box<qt::QPushButton>
    {
        return box<qt::QPushButton>::make(new QPushButton());
    }

    auto QPushButton_setText(box<qt::QPushButton> self, String text) -> void
    {
        (*self)->setText(QString::fromUtf16(text.data(), text.size()));
    }

    auto QPushButton_clicked(box<qt::QPushButton> self, fn<Unit()> cb) -> void
    {
        QObject::connect(self->get(), &QPushButton::clicked, [cb = own{cb}] {
            cb.repr();
        });
    }

    auto QPushButton_as_QWidget(box<qt::QPushButton> self) -> box<qt::QWidget>
    {
        return box<qt::QWidget>::make(qobject_cast<QWidget*>(self.repr->get()));
    }

    auto test_cb(fn<Unit()> cb) -> void
    {
        cb();
    }
}

#pragma clang diagnostic pop
