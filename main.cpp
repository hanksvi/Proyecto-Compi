#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parser.h"
#include "ast.h"
#include "visitor.h"
#include "TypeChecker.h"
using namespace std;

int main(int argc, const char* argv[]) {
    // Verificar número de argumentos
    if (argc != 2) {
        cout << "Número incorrecto de argumentos.\n";
        cout << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }

    // Abrir archivo de entrada
    ifstream infile(argv[1]);
    if (!infile.is_open()) {
        cout << "No se pudo abrir el archivo: " << argv[1] << endl;
        return 1;
    }

    // Leer contenido completo del archivo en un string
    string input, line;
    while (getline(infile, line)) {
        input += line + '\n';
    }
    infile.close();

    // Crear instancias de Scanner 
    Scanner scanner1(input.c_str());
    Scanner scanner2(input.c_str());

    // Generar archivo de tokens
    ejecutar_scanner(&scanner1, argv[1]);

    // Crear instancias de Parser
    Parser parser(&scanner2);

    // Parsear y generar AST
    Program* program = nullptr;
    
    try {
        program = parser.parseProgram();
    } catch (const std::exception& e) {
        cerr << "Error al parsear: " << e.what() << endl;
        return 1;
    }

    // Impresión del AST
    cout << "\n=== Iniciando impresión del programa ===\n";
    PrintVisitor impresion;
    impresion.imprimir(program);

     

    // Generación de código ensamblador
    string inputFile(argv[1]);
    size_t dotPos = inputFile.find_last_of('.');
    string baseName = (dotPos == string::npos) ? inputFile : inputFile.substr(0, dotPos);
    string outputFilename = baseName + ".s";
    ofstream outfile(outputFilename);
    if (!outfile.is_open()) {
        cerr << "Error al crear el archivo de salida: " << outputFilename << endl;
        return 1;
    }

    try {
        cout << "\n ===Iniciando chequeo de tipos===\n";
        TypeChecker tc;
        tc.typecheck(program);
        cout << " Chequeo de tipos exitoso\n";
    } catch (const std::runtime_error& e) {
        cerr << "Error de tipos: " << e.what() << endl;
        return 1;    
    }

    cout << "\n Generando codigo ensamblador en " << outputFilename << "\n";
    GenCodeVisitor codigo(outfile);
    codigo.generar(program);
    outfile.close();

 
    
    
    return 0;
}