cmake_minimum_required(VERSION 3.29)
project(generate_contrato)

set(CMAKE_CXX_STANDARD 20)

# Adicione manualmente os diretórios, se necessário
include_directories(/usr/include)
link_directories(/usr/lib)

# Adicione os arquivos fonte
add_executable(generate_contrato
        main.cpp
        class/DocxToPdfConverter.cpp
)

# Vincule a biblioteca libharu manualmente
target_link_libraries(generate_contrato PRIVATE hpdf)