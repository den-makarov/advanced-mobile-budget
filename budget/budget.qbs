import qbs

CppApplication {
    consoleApplication: true
    files: [
/* My solution for answers referencing */
//        "budget.cpp",
//        "date.cpp",
//        "main.cpp",
//        "budget_good_base.cpp",
//        "stringhelper.cpp",
//        "dates.cpp"
/* Author's solution for answers referencing */
        "desktop_template.cpp"
    ]
    cpp.driverFlags: [
        "-std=c++1z"
    ]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}