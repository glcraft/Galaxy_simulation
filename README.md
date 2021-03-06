# Galaxy_simulation  

## Démonstration de la [simulation de Barnes-Hut](https://fr.wikipedia.org/wiki/Simulation_de_Barnes-Hut)

<!-- ![demo](github/GalaxySimu.png "Démonstration") -->
<img src="github/GalaxySimu.png" style="text-align:center; border-radius: 5em">

## Construction

* Dès que vous avez cloner le projet, lancez la commande suivante `git submodule init`
* Si ce n'est pas déjà fait, installez [vcpkg](https://github.com/microsoft/vcpkg) et [cmake](https://cmake.org)
* Installer les packages suivants via vcpkg :
  * sdl2
  * glew
  * glm
* Configurez le projet via cmake (avec pour toolchain celui de vcpkg)
* Build!

### Exemple de construction sur Windows

Une fois que vous vous êtes prémuni de vcpkg et cmake, et en utilisant Visual Studio 2019 comme IDE, procédez ainsi :

```bat
git submodule init
vcpkg install sdl2:x64-windows glm:x64-windows glew:x64-windows 
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x86 ..
```
Démarrez le projet Visual Studio dans le dossier `build/` puis lancer la compilation.

## Release

Vous pouvez essayer le programme de simulation de galaxie [en le téléchargeant ici](https://github.com/glcraft/Galaxy_simulation/releases)

## Crédits

* [Angel Uriot (as DIMENSION)](https://github.com/angeluriot) - Créateur du projet
* [Gabin Lefranc (as GLCraft)](https://github.com/glcraft) - Amélioration performance.