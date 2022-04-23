cmake_minimum_required(VERSION 3.0.2)

include(Helpers)

FindOrInstallPackageFromFtp("zeromq"  "4.2.0")
FindOrInstallPackageFromFtp("openpgm"  "5.2.127")
