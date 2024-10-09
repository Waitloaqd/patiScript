#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <regex>
#include <thread>  // Para std::this_thread::sleep_for
#include <chrono>  // Para std::chrono::seconds

// Armazena variáveis e seus valores
std::unordered_map<std::string, std::string> variaveis;

// Função para processar o código patiScript e armazenar variáveis
std::vector<std::string> lexer(const std::string &codigo) {
    std::regex re("\\s+|(?<=\\W)|(?=\\W)");
    std::sregex_token_iterator it(codigo.begin(), codigo.end(), re, -1);
    std::vector<std::string> tokens(it, {});
    return tokens;
}

// Função para processar a lógica e substituir variáveis pelos seus valores
std::string obterValor(const std::string& token) {
    if (variaveis.find(token) != variaveis.end()) {
        return variaveis[token];  // Substitui o token pelo valor da variável
    }
    return token;
}

void interpretar(const std::vector<std::string>& tokens, std::ofstream &html) {
    if (tokens[0] == "redi" && tokens[2] == "=") {
        variaveis[tokens[1]] = tokens[3];  // Armazena a variável
    }

    if (tokens[0] == "image.size" && tokens[2] == "=") {
        std::string largura = obterValor(tokens[3]);
        std::string altura = obterValor(tokens[5]);
        html << "<style>img { width: " << largura << "px; height: " << altura << "px; }</style>" << std::endl;
    }

    // Implementando o comando tempo
    if (tokens[0] == "tempo" && tokens[1] == "(" && tokens[2] == ")") {
        int segundos = std::stoi(tokens[3]);  // Converte o valor para inteiro
        std::this_thread::sleep_for(std::chrono::seconds(segundos));  // Pausa a execução
    }

    // Exibir novo texto após o tempo
    if (tokens[0] == "redi" && tokens[1] == "novo_texto" && tokens[2] == "=") {
        std::string texto = tokens[3];
        html << "<p>" << texto << "</p>";  // Adiciona o novo texto ao HTML
    }

    if (tokens[0] == "redi" && tokens[3] == ">+") {
        variaveis[tokens[1]] = std::to_string(std::stoi(variaveis[tokens[1]]) + std::stoi(tokens[4])); // Incrementa valor
    }
}

int main() {
    std::ifstream arquivoPTS("teste.pts");  // Ler o arquivo patiScript
    std::ofstream html("index.html");       // Gerar arquivo HTML de saída
    std::string linha;

    if (arquivoPTS.is_open() && html.is_open()) {
        html << "<!DOCTYPE html>\n<html>\n"; // Começo do HTML
        while (getline(arquivoPTS, linha)) {
            auto tokens = lexer(linha); // Análise léxica
            interpretar(tokens, html);  // Interpretar e gerar HTML
        }
        html << "</html>";              // Fim do HTML
        arquivoPTS.close();
        html.close();
    } else {
        std::cout << "Erro ao abrir o arquivo!" << std::endl;
    }

    return 0;
}
