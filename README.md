### Extensiones para la ejecucion en visual studio y no en clion

- **C/C++** (Microsoft) 
- **CMake Tools** (Microsoft)¡
- **CMake** (twxs) —

 **Importante:** desinstala *C/C++ Runner*, desactívala para este entorno de trabajo ya que invoca `g++` sin leer el `CMakeLists.txt`, por lo que no encuentra los includes de GLAD ni GLFW, y produce errores al no encontrar las rutas

### Configuración de vcpkg

El archivo `.vscode/settings.json` incluye la ruta a vcpkg para que CMake pueda encontrarlo:

```json
{
  "cmake.configureArgs": [
    "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
  ]
}
```

### Como compilar y ejecutar en visual code studio


1. `Ctrl+Shift+P` → **CMake: Configure**. Si pregunta por un kit, seleccionar el
   compilador GCC de MSYS2/UCRT64.
3. `Ctrl+Shift+P` → **CMake: Build** para compilar el poryecto.
4. `Ctrl+Shift+P` → **CMake: Run Without Debugging** para ejecutarlo.

También se puede compilar y ejecutar desde los botones **Build** y **▶** de la barra inferior, equivaldran a los comandos anteriores.


## Ejercicio de reflexión: 

No se admiten métodos de clase como `PAG::Renderer::refrescarVentana()`directamente, porque un método lleva un puntero oculto a `this` que no espera GLFW.

La solución es separar dos capas:

1. **PAG::Renderer** aqui tendremos toda la lógica para dibujar (por ejemplo,
   `refrescarVentana()`), y no sabe nada de GLFW ni fuera de la logica de dibujado — así queda
   desacoplada y reutilizable.

2. Un módulo de "Aplicación" (típicamente `main.cpp` o una clase `PAG::Application`)
   se encarga de:
   - Crear la instancia de `PAG::Renderer`.
   - Guardar un puntero a esa instancia accesible desde las funciones callback,
     bien como variable global/estática, bien mediante
     `glfwSetWindowUserPointer(window, renderer)`.
   - Definir las funciones callback en C como simples "puentes" que delegan
     en el método real del objeto:

```cpp
   void window_refresh_callback(GLFWwindow *window)
   {
       auto *renderer = static_cast<PAG::Renderer*>(glfwGetWindowUserPointer(window));
       renderer->refrescarVentana();
   }
```

Con `glfwSetWindowUserPointer` evitamos una variable global, ya que el
puntero al renderer queda asociado a la propia ventana de GLFW y se recupera
con `glfwGetWindowUserPointer(window)` dentro de cada callback.
