#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <hpdf.h>

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

DocxToPdfConverter::DocxToPdfConverter(const std::string &docxFile, const std::string &pdfFile)
    : docxFile(docxFile), pdfFile(pdfFile), textFile("output.txt") {
}

void DocxToPdfConverter::convert() const {
    convertDocxToText();
    createPDF();
}

void DocxToPdfConverter::convertDocxToText() const {
    std::string command = "docx2txt " + docxFile + " " + textFile;
    int result = system(command.c_str());
    if (result != 0) {
        std::cerr << "Erro ao converter o arquivo DOCX para texto." << std::endl;
        exit(result);
    }

    // Verifique se o arquivo de texto foi gerado e não está vazio
    std::ifstream checkFile(textFile);
    if (!checkFile || checkFile.peek() == std::ifstream::traits_type::eof()) {
        std::cerr << "Arquivo de texto gerado está vazio ou não foi criado." << std::endl;
        exit(EXIT_FAILURE);
    }
    checkFile.close();
}

void DocxToPdfConverter::createPDF() const {
    HPDF_Doc pdf = HPDF_New(NULL, NULL);
    // HPDF_New cria um novo documento PDF e retorna um ponteiro para o documento.
    // Se a criação falhar, retorna NULL.
    if (!pdf) {
        std::cerr << "Erro ao criar o PDF." << std::endl;
        return;
    }

    HPDF_UseUTFEncodings(pdf);
    // HPDF_UseUTFEncodings ativa o suporte para codificação UTF-8 no documento PDF.
    // Isso permite que o PDF use caracteres especiais e diferentes idiomas.

    HPDF_Page page = HPDF_AddPage(pdf);
    // HPDF_AddPage adiciona uma nova página ao documento PDF e retorna um ponteiro para a nova página.

    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
    // HPDF_Page_SetSize define o tamanho da página. Aqui, definimos o tamanho A4 (210mm x 297mm) com a orientação retrato (vertical).

    const char* font = HPDF_LoadTTFontFromFile(pdf, "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", HPDF_TRUE);
    // HPDF_LoadTTFontFromFile carrega uma fonte TrueType do arquivo fornecido e registra-a no documento PDF.
    // O terceiro argumento, HPDF_TRUE, indica que a fonte será embutida no PDF, ou seja, a fonte será incluída no arquivo PDF.

    if (!font) {
        std::cerr << "Erro ao carregar a fonte." << std::endl;
        HPDF_Free(pdf);
        // HPDF_Free libera os recursos associados ao documento PDF.
        return;
    }

    HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, font, "UTF-8"), 12);
    // HPDF_SetFontAndSize define a fonte e o tamanho do texto na página.
    // HPDF_GetFont obtém a fonte do documento PDF registrada anteriormente. Aqui, estamos definindo o tamanho da fonte como 12.

    std::ifstream inputFile(textFile);
    // Abre o arquivo de texto gerado pela função convertDocxToText para leitura.

    if (!inputFile) {
        std::cerr << "Erro ao abrir o arquivo de texto." << std::endl;
        HPDF_Free(pdf);
        return;
    }

    std::string line;
    float y = HPDF_Page_GetHeight(page) - 50;
    // HPDF_Page_GetHeight obtém a altura da página. Aqui usamos essa altura para calcular a posição Y inicial do texto.
    const float leftMargin = 50;
    // Define a margem esquerda da página.
    const float lineHeight = 15;
    // Define o espaçamento vertical entre as linhas de texto.
    const float paragraphSpacing = 30;
    // Define o espaçamento entre parágrafos.
    const float pageWidth = HPDF_Page_GetWidth(page) - leftMargin * 2;
    // HPDF_Page_GetWidth obtém a largura da página. Aqui usamos isso para calcular a largura útil da página, descontando as margens.

    // Adicione texto ao PDF linha por linha
    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            y -= paragraphSpacing;
            continue;
        }

        float textWidth = HPDF_Page_TextWidth(page, line.c_str());
        // HPDF_Page_TextWidth calcula a largura do texto em pontos com base na fonte e no tamanho atuais.

        // Quebra de linha se o texto for muito longo
        while (textWidth > pageWidth) {
            size_t lastSpace = line.find_last_of(' ', line.length() * pageWidth / textWidth);
            if (lastSpace == std::string::npos) {
                lastSpace = line.length();
            }
            HPDF_Page_BeginText(page);
            // HPDF_Page_BeginText inicia a inserção de um novo bloco de texto.
            HPDF_Page_TextOut(page, leftMargin, y, line.substr(0, lastSpace).c_str());
            // HPDF_Page_TextOut escreve texto na página nas coordenadas X (margem esquerda) e Y (posição atual).
            HPDF_Page_EndText(page);
            // HPDF_Page_EndText finaliza a inserção do bloco de texto.

            y -= lineHeight;
            // Diminui a posição Y para escrever a próxima linha abaixo da atual.
            line = line.substr(lastSpace + 1);
            // Atualiza a linha de texto removendo a parte que já foi escrita.
            textWidth = HPDF_Page_TextWidth(page, line.c_str());

            if (y < 50) {
                y = HPDF_Page_GetHeight(page) - 50;
                page = HPDF_AddPage(pdf);
                HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
                HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, font, "UTF-8"), 12);
            }
        }

        if (!line.empty()) {
            HPDF_Page_BeginText(page);
            HPDF_Page_TextOut(page, leftMargin, y, line.c_str());
            HPDF_Page_EndText(page);
            y -= lineHeight;
        }

        if (y < 50) {
            y = HPDF_Page_GetHeight(page) - 50;
            page = HPDF_AddPage(pdf);
            HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
            HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf, font, "UTF-8"), 12);
        }
    }

    HPDF_SaveToFile(pdf, pdfFile.c_str());
    // HPDF_SaveToFile salva o documento PDF no arquivo fornecido.

    std::cout << "PDF salvo como: " << pdfFile << std::endl;

    HPDF_Free(pdf);
    // HPDF_Free libera todos os recursos associados ao documento PDF.
    inputFile.close();
    // Fecha o arquivo de entrada de texto.
}
