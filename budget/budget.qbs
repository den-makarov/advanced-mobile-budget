import qbs

CppApplication {
    consoleApplication: true
    files: [
/* My solution for answers referencing */
//        "ref/budget.cpp",
//        "ref/date.cpp",
//        "ref/main.cpp",
/* My advanced solution based on author's template */
        "main.cpp",
        "src/stringhelper.cpp",
        "src/dates.cpp",
        "src/moneystate.cpp",
        "src/indexsegment.cpp",
        "src/compute_request.cpp",
        "src/add_request.cpp",
        "src/pay_tax_request.cpp"
/* Author's solution for answers referencing */
//        "ref/desktop_template.cpp"
    ]
    cpp.driverFlags: [
        "-std=c++1z"
    ]
    cpp.includePaths: ["inc"]

    Group {     // Properties for the produced executable
        fileTagsFilter: "application"
        qbs.install: true
        qbs.installDir: "bin"
    }
}
