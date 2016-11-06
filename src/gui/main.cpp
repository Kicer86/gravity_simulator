
#include <fenv.h>

#include <QApplication>

#include "main_window.hpp"
#include "simulation_engine.hpp"


int main(int argc, char** argv)
{
    feenableexcept(FE_INVALID | FE_OVERFLOW);

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return app.exec();
}
