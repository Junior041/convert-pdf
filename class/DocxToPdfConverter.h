#ifndef DOCX_TO_PDF_CONVERTER_H
#define DOCX_TO_PDF_CONVERTER_H

#include <string>

class DocxToPdfConverter {
public:
    DocxToPdfConverter(const std::string &docxFile, const std::string &pdfFile);
    void convert() const;

private:
    std::string docxFile;
    std::string pdfFile;
    std::string textFile;

    void convertDocxToText() const;
    void createPDF() const;
};

#endif // DOCX_TO_PDF_CONVERTER_H
