import os
import subprocess
import shutil

# Archivos c++
programa = ["main.cpp", "scanner.cpp", "token.cpp", "parser.cpp", "ast.cpp", "visitor.cpp"]

# Compilar
compile = ["g++"] + programa
print("Compilando:", " ".join(compile))
result = subprocess.run(compile, capture_output=True, text=True)

if result.returncode != 0:
    print("Error en compilación:\n", result.stderr)
    exit(1)

print("Compilación exitosa")

# Ejecutar
input_dir = "inputs"
output_dir = "outputs"
os.makedirs(output_dir, exist_ok=True)

for i in range(1, 7):
    filename = f"input{i}.txt"
    filepath = os.path.join(input_dir, filename)

    if os.path.isfile(filepath):
        print(f"\n{'='*50}")
        print(f"Procesando {filename}")
        print('='*50)
        
        run_cmd = ["./a.out", filepath]
        result = subprocess.run(run_cmd, capture_output=True, text=True)

        # Guardar stdout y stderr
        output_file = os.path.join(output_dir, f"output{i}.txt")
        with open(output_file, "w", encoding="utf-8") as f:
            f.write("=== STDOUT ===\n")
            f.write(result.stdout)
            f.write("\n=== STDERR ===\n")
            f.write(result.stderr)

        print(f"✓ Output guardado en: {output_file}")

        # Archivos generados
        tokens_file = os.path.join(input_dir, f"input{i}_tokens.txt")
        ast_file = "ast.dot"

        # Mover archivo de tokens si existe
        if os.path.isfile(tokens_file):
            dest_tokens = os.path.join(output_dir, f"tokens_{i}.txt")
            shutil.move(tokens_file, dest_tokens)
            print(f"✓ Tokens guardados en: {dest_tokens}")

        # Mover y convertir AST si existe
        if os.path.isfile(ast_file):
            dest_ast = os.path.join(output_dir, f"ast_{i}.dot")
            shutil.move(ast_file, dest_ast)
            print(f"✓ AST (.dot) guardado en: {dest_ast}")

            # Convertir a PNG
            output_img = os.path.join(output_dir, f"ast_{i}.png")
            dot_result = subprocess.run(
                ["dot", "-Tpng", dest_ast, "-o", output_img],
                capture_output=True,
                text=True
            )
            if dot_result.returncode == 0:
                print(f"✓ AST (.png) generado en: {output_img}")
            else:
                print(f"✗ Error al generar PNG del AST")

        # Mover archivo .s si existe
        asm_file = os.path.join(input_dir, f"input{i}.s")
        if os.path.isfile(asm_file):
            dest_asm = os.path.join(output_dir, f"assembly_{i}.s")
            shutil.move(asm_file, dest_asm)
            print(f"✓ Assembly guardado en: {dest_asm}")

    else:
        print(f"✗ {filename} no encontrado en {input_dir}")

print(f"\n{'='*50}")
print("Proceso completado")
print('='*50)