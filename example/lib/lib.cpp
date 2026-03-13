#include <moonbridge.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSharedPointer>
#include <QWidget>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
#pragma clang diagnostic ignored "-Wunknown-pragmas" // 不加这行会导致 pop 报警告，我也不知道为什么

namespace qt
{

using QObject      = QSharedPointer<QObject>;
using QWidget      = QSharedPointer<QWidget>;
using QLayout      = QSharedPointer<QLayout>;
using QApplication = QSharedPointer<QApplication>;
using QHBoxLayout  = QSharedPointer<QHBoxLayout>;
using QLabel       = QSharedPointer<QLabel>;
using QPushButton  = QSharedPointer<QPushButton>;

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
            auto u8s  = u16s.toStdString();
            Args.argarr.push_back(std::move(u8s));
            Args.argv.push_back(Args.argarr.back().data());
        }
        Args.argv.push_back(nullptr);

        auto&& argc = Args.argc;
        auto&& argv = Args.argv.data();
        return box<qt::QApplication>::make(qt::QApplication::create(argc, argv));
    }

    auto QApplication_exec(box<qt::QApplication> self) -> Int
    {
        return (*self)->exec();
    }

    auto QApplication_as_QObject(box<qt::QApplication> self) -> box<qt::QObject>
    {
        return box<qt::QObject>::make(self.repr->objectCast<QObject>());
    }

    auto QWidget_new() -> box<qt::QWidget>
    {
        return box<qt::QWidget>::make(qt::QWidget::create());
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
        return box<qt::QObject>::make(self.repr->objectCast<QObject>());
    }

    auto QWidget_setLayout(box<qt::QWidget> self, box<qt::QLayout> layout)
    {
        (*self)->setLayout(layout->get());
    }

    auto QHBoxLayout_new() -> box<qt::QHBoxLayout>
    {
        return box<qt::QHBoxLayout>::make(qt::QHBoxLayout::create());
    }

    auto QHBoxLayout_addWidget(box<qt::QHBoxLayout> self, box<qt::QWidget> widget) -> void
    {
        (*self)->addWidget(widget->get());
    }

    auto QHBoxLayout_as_QLayout(box<qt::QHBoxLayout> self) -> box<qt::QLayout>
    {
        return box<qt::QLayout>::make(self.repr->objectCast<QLayout>());
    }

    auto QLabel_new() -> box<qt::QLabel>
    {
        return box<qt::QLabel>::make(qt::QLabel::create());
    }

    auto QLabel_setText(box<qt::QLabel> self, String text) -> void
    {
        (*self)->setText(QString::fromUtf16(text.data(), text.size()));
    }

    auto QLabel_as_QWidget(box<qt::QLabel> self) -> box<qt::QWidget>
    {
        return box<qt::QWidget>::make(self.repr->objectCast<QWidget>());
    }

    auto QPushButton_new() -> box<qt::QPushButton>
    {
        return box<qt::QPushButton>::make(qt::QPushButton::create());
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
        return box<qt::QWidget>::make(self.repr->objectCast<QWidget>());
    }

    auto test_cb(fn<Unit()> cb) -> void
    {
        cb();
    }
}

#pragma clang diagnostic pop
