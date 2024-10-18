#include <iostream>
#include "class/DocxToPdfConverter.h"

int main() {
    const std::string docxFile = "../CONTRATO.docx"; // Caminho para o arquivo DOCX
    const std::string pdfFile = "output.pdf"; // Arquivo PDF de saída

    // Cria uma instância do conversor e realiza a conversão
    DocxToPdfConverter converter(docxFile, pdfFile);
    converter.convert();

    std::cout << "PDF gerado com sucesso!" << std::endl;

    return 0;
}
