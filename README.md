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


## Ejercicio de reflexión

### El problema

GLFW espera normalmente firmas concretas, no cpn firmas pertenecientes a C, por ejemplo:

```cpp
void window_refresh_callback(GLFWwindow *window);
```

Por lo que no se admiten métodos de como `PAG::Renderer::refrescarVentana()`
ya que un método lleva un puntero oculto a `this` que no espera
GLFW.

### La solución

La solución es separar dos capas:

1. **PAG::Renderer** aquí tendremos toda la lógica para dibujar (por ejemplo,
   `refrescarVentana()`), y no sabe nada de GLFW ni fuera de la lógica de
   dibujado, así será completamente desacoplada y reutilizable. 

2. Un módulo que se encargará de:
   - Crear la instancia de `PAG::Renderer`.
   - Guardar un puntero para acceder desde las funciones, como `glfwSetWindowUserPointer(window, renderer)`.


```cpp
   void window_refresh_callback(GLFWwindow *window)
   {
       auto *renderer = static_cast<PAG::Renderer*>(glfwGetWindowUserPointer(window));
       renderer->refrescarVentana();
   }
```

Aquí se puede observar:

- `glfwGetWindowUserPointer(window)` le pregunta a la ventana para que le devuelva un `void*` (un puntero genérico ya que GLFW no sabe qué clase es).
- `static_cast<PAG::Renderer*>(...)` el static cast realmente le dice al compilador que el tipo será `PAG::Renderer*`", y así llamar a sus métodos con normalidad.

Con `glfwSetWindowUserPointer` evitamos una variable global, ya que el
puntero apuntará a la ventana de GLFW y se recupera con `glfwGetWindowUserPointer(window)` que devuelve el puntero genérico así si en algún momento tuvieras varias ventanas abiertas a la vez, cada una podría tener su propio renderer asociado. Con una variable global solo podrías tener un renderer "activo" para toda la aplicación.

### Diagrama de clases 
![diagrama](MAIN.png)


# Práctica 2: patrones Singleton y Observador
### Cambios realizados respecto a la Práctica 1

Creamos la clase renderer usando el patron singleton, que centraliza todas las llamadas a OpenGL. main.cpp ya no llama a ninguna función de OpenGL directamente, solo a GLFW, GLAD y Renderer.
Comenzamos a trabajar con Dear ImGui como biblioteca de interfaz, encapsulada en la clase GUI (también Singleton), que centraliza toda la comunicación con ImGui.
Añadimos el patrón Observador para comunicar los cambios de la interfaz a Renderer, gracias a la interfaz PAG::Listener y el método wakeUp() que tendrá parámetros variables para que en un futuro podamos trabajar con distintos casos.
Ahora mostraremos los mensajes que antes estaban en la consola en una ventana flotante de ImGui.
El bucle principal ahora será continuo ya que Dear ImGui debe reconstruirse en cada fotograma.

### Patrón Singleton

Una clase tendrá una única instancia en toda la ejecución, accesible desde cualquier lugar. Se implementa con:

Un atributo static privado, puntero a la propia clase, la unica instancia.
Un constructor privado, que impide crear objetos desde fuera de la clase.
Un método público y estático getInstancia() el cual verifica si ya se ha creado o crea el objeto. Esto es importante porque Renderer necesita que ya exista un contexto OpenGL activo antes de poder configurarse.


### Patrón Observador

Cambiamos el funcionamiento para notificar a los listeners cuando el color del fondo cambia sin que esos objetos estén constantemente preguntando si algo ha cambiado.
 En su lugar, es el propio objeto observado quien avisa activamente en el instante exacto del cambio.

Implementacion:

Listener: interfaz implementada mediante una clase abstracta con un único método virtual puro, wakeUp, que deben implementar todos los observadores.
WindowType: enum que identificará el tipo y por tanto origen de cada notificación.
GUI: será sujeto observable. Tiene un vector<Listener*> _listeners con los observadores suscritos. Cuando el el color del fondo sea modificado, llamará a wakeUp en cada observador suscrito.
Renderer: hereda de Listener e implementa wakeUp, procesando la notificación según el WindowType recibido y actualizando su color de fondo interno.

La suscripción se realiza en main.cpp:

Dear ImGui es una biblioteca de interfaz sin objetos persistentes entre fotogramas, en cada vuelta del bucle principal se vuelve a "describir" la interfaz completa. Por este motivo, el dibujado de la interfaz pasa a depender de un bucle continuo, en lugar de eventos de refresco de GLFW.

Toda esta comunicación queda encapsulada en GUI, de forma que si en el futuro se cambiara de biblioteca de interfaz, solo habría que modificar esa clase.

### Diagrama de clases 
![alt text](UML-P2.png)

Gui tendrá una lista de objets de tipo listener dentro de una lista, cada vez que el color de fondo sea cambiado, recorrerá esta lista y notificará a todos los sucritos mediante la función wakeup. Listener se encuentra implementado en renderer por lo que al recibir la notificación actualizará el color interno llamando a glClearColor

# Práctica 3: Renderizando nuestro primer triángulo

## Cambios realizados respecto a la Práctica 2

- `PAG::Renderer` incorpora los identificadores necesarios para gestionar
  shaders y geometría en la GPU (`idVS`, `idFS`, `idSP`, `idVAO`, `idVBO`,
  `idVBOColores`, `idIBO`).
- Ahora creamos, compilamos y enlazamos los shaders en
  `creaShaderProgram`, creando un modelo básico `creaModelo`: un triángulo definido en coordenadas canónicas.
- El código GLSL se ha movido a ficheros de texto (`shaders/pag03-vs.glsl` y
  `shaders/pag03-fs.glsl`), que `Renderer` (`leerFichero`). 
  El `CMakeLists.txt` copia la carpeta `shaders/` junto al ejecutable con cada build.
- Se ha añadido comprobación de errores (ejercicio 2) en la compilación y el enlazado de
  los shaders. `Renderer` lanzará una excepción (`std::runtime_error`) con el mensaje 
  de error que devuelve el driver de OpenGL (se registra en el log).
   `main.cpp` captura esa excepción y envía el mensaje a la ventana "Mensajes"
   de la interfaz.
- Para el apartado opcional hemo añadido un segundo atributo de vértice con el color, 
  para que el vertex shader se lo pase al fragment shader y este lo asigne al fragmento. 
  Así tendremos un triángulo con un gradiente de color.
  Se incluyen las dos versiones que pide el enunciado: VBOs no entrelazados
  (activa) y un único VBO entrelazado (comentada en el mismo método).
- Hemos resuelto el problema al redimensionar la ventana con la proporciones
  del triángulo aplicando una matriz de corrección de aspecto en el vertex shader,


La separación de responsabilidades establecida en la Práctica 2 se
mantiene sin cambios: `main.cpp` sigue sin contener ninguna llamada a
OpenGL. Toda la lógica nueva (shaders, geometría, matriz de corrección)
vive dentro de `PAG::Renderer`, que sigue siendo la única clase que habla
con la API gráfica.


## Gestión de errores de shaders

Un error de sintaxis no provoca ningún error de compilación del proyecto; sin
comprobación explícita, el único síntoma visible sería que el triángulo no
aparece, sin ninguna pista del motivo.

`Renderer::compilarShader` realiza comprobaciones cada vez que refresca la pantalla 
tras cada `glCompileShader` mediante la funcion `Renderer::creaShaderProgram`.
Si alguna comprobación falla, se recupera el mensaje de error del driver,
se liberan los recursos ya creados para no dejar objetos a medias en la GPU, y se lanza una
excepcion de tipo `std::runtime_error` con el mensaje.
`main.cpp` captura la excepción con un `try/catch` alrededor de la creación del shader program y del modelo,
reenviando el mensaje a la ventana "Mensajes" de la interfaz.

Para poder demostrar este funcionamiento `Renderer.h` incluye una constante `FORZAR_ERROR_DEMO`, 
que al activarse introduce una línea de GLSL inválida en el fragment shader, provocando un error de compilación
reproducible.

## Respuesta a la pregunta 5 

Si redimensionas la ventana de la aplicación, el triángulo
se deforma. ¿A qué se debe este comportamiento?

La geometría del triángulo está definida en coordenadas canónicas, es decir, 
el rango [-1, 1] en los ejes X e Y, y el vertex shader no
aplica ninguna transformación: copia la posición de cada vértice
directamente a `gl_Position`. OpenGL toma esas coordenadas y las estira
para ocupar todo el *viewport*, cuyo tamaño se ajusta en vez que se redimensiona
con `glViewport`. Si la ventana no mantiene la proporción (ancho/alto) con la que se definió la geometría, el
estiramiento aplicado en el eje X es distinto al aplicado en el eje Y, y el triángulo se deforma.

Para solucionar este problema eviaremos al vertex shader una matriz de escalado que compensa esa
diferencia: si la ventana es más ancha que alta, se reduce la coordenada
X en la misma proporción en que el viewport la va a estirar de más; si es
más alta que ancha, se reduce la Y. La matriz se construye manualmente
como un array de 16, en el orden que espera OpenGL

Despues `_aspect` se actualizará en `Renderer::redimensionar` cada vez que cambia el
tamaño de la ventana, y la matriz se recalcula en cada `refrescar()` con `glUniformMatrix4fv`.

## Diagrama de clases (UML, ampliación de la Práctica 2)

![alt text](UML-P3.png)

`PAG::Renderer` sigue siendo la única clase que interactua con OpenGL y GLSL.
`main.cpp` la invoca a través de sus métodos públicos y captura las
excepciones, reenviando el mensaje a `PAG::GUI` para
mostrarlo en la ventana de mensajes, sin que `Renderer` necesite conocer
dónde ni cómo se muestra ese mensaje.

