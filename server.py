#!/usr/bin/env python3
import http.server
import socketserver
import os
import subprocess
import json
import tempfile
from pathlib import Path

# ==== CONFIGURACIÓN BÁSICA ====
PORT = 8000

# Ruta raíz del proyecto (ajústala si tu carpeta cambia)
PROJECT_ROOT = r"c:\acompi\Proyecto-Compi"
INPUT_DIR = Path(PROJECT_ROOT) / "inputs"


class CompilerHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/compile':
            # Leer JSON del frontend
            content_length = int(self.headers.get('Content-Length', 0))
            post_data = self.rfile.read(content_length)
            try:
                request = json.loads(post_data.decode('utf-8'))
            except json.JSONDecodeError:
                self._send_json({
                    'exitoso': False,
                    'stdout': '',
                    'stderr': 'Error: JSON inválido en la petición.',
                    'assembly': ''
                })
                return

            codigo = request.get('codigo', '')

            # Asegurar carpeta inputs/
            INPUT_DIR.mkdir(parents=True, exist_ok=True)

            # Guardar código en archivo temporal dentro de inputs/
            try:
                with tempfile.NamedTemporaryFile(
                    mode='w', suffix='.txt', delete=False, dir=INPUT_DIR
                ) as f:
                    f.write(codigo)
                    archivo_entrada = f.name  # ruta completa al .txt
            except Exception as e:
                self._send_json({
                    'exitoso': False,
                    'stdout': '',
                    'stderr': f'Error creando archivo temporal: {e}',
                    'assembly': ''
                })
                return

            try:
                # Ejecutar compilador (programa.exe o programa)
                exe_path = Path(PROJECT_ROOT) / "programa.exe"
                if not exe_path.exists():
                    exe_path = Path(PROJECT_ROOT) / "programa"

                if not exe_path.exists():
                    self._send_json({
                        'exitoso': False,
                        'stdout': '',
                        'stderr': f'No se encontró el ejecutable: {exe_path}',
                        'assembly': ''
                    })
                    return

                # Llamar al compilador con el archivo de entrada
                resultado = subprocess.run(
                    [str(exe_path), archivo_entrada],
                    cwd=PROJECT_ROOT,
                    capture_output=True,
                    text=True,
                    timeout=10
                )

                # El main genera el .s en la MISMA ruta que el .txt
                entrada_path = Path(archivo_entrada)
                archivo_assembly = entrada_path.with_suffix('.s')

                assembly_content = ''
                if archivo_assembly.exists():
                    assembly_content = archivo_assembly.read_text(encoding='utf-8')
                else:
                    # No es fatal, pero lo avisamos
                    resultado.stderr += (
                        f"\n[server.py] Aviso: no se encontró el archivo de ensamblador: "
                        f"{archivo_assembly}"
                    )

                respuesta = {
                    'exitoso': resultado.returncode == 0,
                    'stdout': resultado.stdout,
                    'stderr': resultado.stderr,
                    'assembly': assembly_content,
                    'archivo_entrada': archivo_entrada
                }

            except Exception as e:
                respuesta = {
                    'exitoso': False,
                    'stdout': '',
                    'stderr': f'Error ejecutando el compilador: {e}',
                    'assembly': ''
                }

            # Enviar respuesta JSON al frontend
            self._send_json(respuesta)

        else:
            # Para cualquier otra ruta POST que no sea /compile
            super().do_GET()

    # Helper para enviar JSON con CORS
    def _send_json(self, data: dict):
        self.send_response(200)
        self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()
        self.wfile.write(json.dumps(data).encode('utf-8'))

    # CORS básico
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        self.end_headers()


if __name__ == '__main__':
    # Servir archivos estáticos (visualizador.html) desde la raíz del proyecto
    os.chdir(PROJECT_ROOT)
    with socketserver.TCPServer(("", PORT), CompilerHandler) as httpd:
        print(f"Servidor ejecutándose en http://localhost:{PORT}/visualizador.html")
        print("Presiona Ctrl+C para detener")
        httpd.serve_forever()
