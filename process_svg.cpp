//
// Created by tomokazu on 2022/02/12.
//

// https://qiita.com/m4saka/items/1e2c24e80212a4c4c584
// https://www.kishiro.com/programming/c/import_cpp.html

// extern "C"
#include <iostream>
#include <python3.9/Python.h>
#include <boost/python.hpp>

namespace py = boost::python;

int main(int argc, char *argv[]) {
    std::string arg = argv[1];
    Py_Initialize();
    try {
        py::object global = py::import("__main__").attr("__dict__");
        py::object os = py::import("os");
        py::object bs4 = py::import("bs4");
        py::object builtins = py::import("builtins");
        py::object io = py::import("io");
        py::exec("print('Hello!')");
        //py::object basename =
        py::exec("basename=" "'test.py'", global);
        // py::object basename = global["basename"];
        // std::string message = py::extract<std::string>(basename);
        // std::cout << message << std::endl;
        py::object filename = os.attr("path").attr("join")(os.attr("getcwd"), global["basename"]);
        //py::object filename = py::eval("'aaaaaaaa'",global);
        py::object filenameStr = builtins.attr("str")(filename);
        std::string fileStr = py::extract<std::string>(filenameStr);
        std::cout << fileStr << std::endl;
    }
    catch (const py::error_already_set &) {
        // Pythonコードの実行中にエラーが発生した場合はエラー内容を表示
        PyErr_Print();
    }

    return 0;
}