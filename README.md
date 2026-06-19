# Godot-based-shader-testbed

- [English](#english)
- [Español](#español)

---

## 🇬🇧 English

This project extends GLarena, the viewer used in the Graphics course at FIB, by integrating Godot as a complementary environment for shader experimentation. It introduces automatic scene generation in Godot, GLSL-to-GDShader translation through glslang AST traversal, JSON-based translation tables, automatic UV generation for unsupported models, and a preconfigured visualization environment with multiple cameras and lighting. The goal is to provide students with a more visual and interactive way to learn and experiment with shaders.


---

Este proyecto amplía GLarena, el visor utilizado en la asignatura de Gráficos de la FIB, integrando Godot como un entorno complementario para la experimentación con shaders.

Las principales aportaciones del proyecto son:

- Generación automática de escenas en Godot.
- Traducción de shaders de GLSL a GDShader mediante el recorrido del Árbol de 
- Sintaxis Abstracta (AST) generado por glslang.
- Uso de tablas de traducción basadas en archivos JSON.
- Generación automática de coordenadas UV para modelos que no disponen de ellas.
- Entorno de visualización preconfigurado con múltiples cámaras e iluminación.

El objetivo es proporcionar a los estudiantes una forma más visual e interactiva de aprender y experimentar con shaders, introducirlos al motor de videojuegos Godot y fomentar el uso de software open source.

Dentro del directorio viewer se incluyen las instrucciones originales de GLarena correspondientes a la versión del repositorio descargada el 24 de febrero de 2026. Asimismo, se incluye el contenido original del repositorio glslang, correspondiente a la revisión realizada en esa misma fecha, junto con su README y la documentación de uso proporcionada por sus autores.

En este documento se describen las dependencias necesarias para compilar y ejecutar el proyecto, así como los pasos para su utilización. Para información adicional, se recomienda consultar la documentación oficial de los proyectos originales.

#### Recursos oficiales
- Khronos Group — https://www.khronos.org
- Virvig — https://www.virvig.eu

### Requisitos

Nota: El proyecto fue desarrollado sobre Ubuntu 24.04 LTS. Aunque esta es la plataforma de referencia, es posible utilizar otras distribuciones de Linux siempre que dispongan de las dependencias indicadas a continuación.

- Sistema operativo Linux
- Qt 5.4 o superior
- Compilador compatible con C++17 (GCC o Clang)
- Godot 4.x


