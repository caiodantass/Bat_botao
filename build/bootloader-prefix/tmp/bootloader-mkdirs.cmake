# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/Steam/Espressif/frameworks/esp-idf-v5.2.2/components/bootloader/subproject"
  "D:/meu_projeto_esp/build/bootloader"
  "D:/meu_projeto_esp/build/bootloader-prefix"
  "D:/meu_projeto_esp/build/bootloader-prefix/tmp"
  "D:/meu_projeto_esp/build/bootloader-prefix/src/bootloader-stamp"
  "D:/meu_projeto_esp/build/bootloader-prefix/src"
  "D:/meu_projeto_esp/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/meu_projeto_esp/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/meu_projeto_esp/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
