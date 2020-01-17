## STF

**STF** is a personal Training Engine with **Modular GPU Particle System implementation**.
A great architecture allows "fast" and "nice" implementation/test for myself.
The engine allows few systems.
- Particle Point
	> Each particle is a little point
	
- Particle Mesh
	>Each particle is a Mesh
	
- Particle Sprite (atlas allowed)
	>Each particle is a Sprite, face oriented to Camera

And a special one :
- Particle SPH (Quick implementation)
	> Smoothed Particle Hydrodynamics : Fluid simulation

### video

## Next
- Boids
- GPU Radix better | Make a paper for explain
- Better SPH
- Full Profiling code, with visual interpretation
- High-Quality volume rendering SPH

## Documentation who helped me

Huge help for SPH : https://wickedengine.net/2018/05/21/scalabe-gpu-fluid-simulation/


## Depedencies
Almost depedencies will automaticly clone/build with cmake.

- (should remove it next) boost [link](https://github.com/Orphis/boost-cmake)
- assimp [link](https://github.com/assimp/assimp)
- glad [link](https://github.com/Dav1dde/glad)
- glm [link](https://github.com/g-truc/glm)
- stb [link](https://github.com/nothings/stb)
- glfw [link](https://github.com/glfw/glfw)
- OpenCL-CLHPP [link](https://github.com/KhronosGroup/OpenCL-CLHPP)
- imgui [link](https://github.com/ocornut/imgui)
- (useless) libnoise [link](https://github.com/qknight/libnoise)
- json [link](https://github.com/nlohmann/json)


## How to Build
### Windows
Install [CUDA ToolKit ](https://developer.nvidia.com/cuda-downloads)
`git clone this repo`
Open  with an IDE (Visual Studio Code/Community..., CLion ...)
or
`mkdir bluid && cd build`
`cmake ../CMakeLists.txt`


## Architecture

A little explanation about pattern who i choose.
Is not very interesting, and seems to be really logic !
But its a praticale explanation


[![](https://mermaid.ink/img/eyJjb2RlIjoiZ3JhcGggVEQ7XG4gICAgUGFydGljbGVTeXN0ZW1fQS0tPlBhcnRpY2xlRW1pdHRlcl9BQTtcbiAgICBQYXJ0aWNsZUVtaXR0ZXJfQUEtLT5QYXJ0aWNsZU1vZHVsZV9BQUFcbiAgICBQYXJ0aWNsZUVtaXR0ZXJfQUEtLT5QYXJ0aWNsZU1vZHVsZV9BdHRyYWN0b3JcbiAgICBQYXJ0aWNsZUVtaXR0ZXJfQUEtLT5QYXJ0aWNsZU1vZHVsZV9HcmF2aXR5XG4gICAgUGFydGljbGVFbWl0dGVyX0FBLS0-UGFydGljbGVNb2R1bGVfUmVzb2x2ZVBoeXNpY1xuICAgIFBhcnRpY2xlRW1pdHRlcl9BQS0tPmlkMVxuXHRpZDEoKEdwdUJ1ZmZlcikpLS0-aWQyPlBhcnRpY2xlRGF0YUJhc2ljXVxuXHRpZDItLT5Qb3NpdGlvblxuXHRpZDItLT5MaWZldGltZVxuXHRpZDItLT5BZ2Vcblx0aWQyLS0-Li4uXG5cdGlkMSgoR3B1QnVmZmVyKSktLT5pZDM-UGFydGljbGVEYXRhUGh5c2ljXVxuXHRpZDMtLT5WZWxvY2l0eVxuXHRpZDMtLT5NYXNzIiwibWVybWFpZCI6eyJ0aGVtZSI6ImZvcmVzdCJ9fQ)](https://mermaid-js.github.io/mermaid-live-editor/#/edit/eyJjb2RlIjoiZ3JhcGggVEQ7XG4gICAgUGFydGljbGVTeXN0ZW1fQS0tPlBhcnRpY2xlRW1pdHRlcl9BQTtcbiAgICBQYXJ0aWNsZUVtaXR0ZXJfQUEtLT5QYXJ0aWNsZU1vZHVsZV9BQUFcbiAgICBQYXJ0aWNsZUVtaXR0ZXJfQUEtLT5QYXJ0aWNsZU1vZHVsZV9BdHRyYWN0b3JcbiAgICBQYXJ0aWNsZUVtaXR0ZXJfQUEtLT5QYXJ0aWNsZU1vZHVsZV9HcmF2aXR5XG4gICAgUGFydGljbGVFbWl0dGVyX0FBLS0-UGFydGljbGVNb2R1bGVfUmVzb2x2ZVBoeXNpY1xuICAgIFBhcnRpY2xlRW1pdHRlcl9BQS0tPmlkMVxuXHRpZDEoKEdwdUJ1ZmZlcikpLS0-aWQyPlBhcnRpY2xlRGF0YUJhc2ljXVxuXHRpZDItLT5Qb3NpdGlvblxuXHRpZDItLT5MaWZldGltZVxuXHRpZDItLT5BZ2Vcblx0aWQyLS0-Li4uXG5cdGlkMSgoR3B1QnVmZmVyKSktLT5pZDM-UGFydGljbGVEYXRhUGh5c2ljXVxuXHRpZDMtLT5WZWxvY2l0eVxuXHRpZDMtLT5NYXNzIiwibWVybWFpZCI6eyJ0aGVtZSI6ImZvcmVzdCJ9fQ)

### An example

We will create somes Physic who influed on each particle. Even somes simple collision on primive shape !
But, for now, we will just create :
- **ParticleModuleGravity**
	>attract particles to a specified direction.
	
- **ParticleModuleAttract**
	> attract particles to a specified position.
	
- **ParticleModuleResolvePhysic**
	>compute new position of particle

Each module needed more than **ParticleDataBasic** information for doing physic.
So **ParticleModuleResolvePhysic** will add **ParticleDataPhysic**, who **ParticleModuleGravity** and **ParticleModuleAttract** needed to !

### Modules constructions
[![](https://mermaid.ink/img/eyJjb2RlIjoiZ3JhcGggVEJcbnN1YmdyYXBoIEVtaXR0ZXIgU3Ryb3JlIEJ1ZmZlcnNcblx0YkJhc2ljPlBhcnRpY2xlRGF0YUJhc2ljXS0tPmlkMVsoR3B1QnVmZmVyKV1cblx0YlBoeXNpYz5QYXJ0aWNsZURhdGFQaHlzaWNdXG5lbmRcblxuc3ViZ3JhcGggM1xuICBQYXJ0aWNsZU1vZHVsZV9SZXNvbHZlUGh5c2ljLS0-QWRkLS4gQWRkIFBhcnRpY2xlRGF0YVBoeXNpYyAuLT5pZDEtLT5iUGh5c2ljXG5lbmRcblxuc3ViZ3JhcGggMlxuICBQYXJ0aWNsZU1vZHVsZV9HcmF2aXR5XG5lbmRcblxuc3ViZ3JhcGggMVxuICBQYXJ0aWNsZU1vZHVsZV9BdHRyYWN0b3JcbmVuZFxuXG5cdCIsIm1lcm1haWQiOnsidGhlbWUiOiJkZWZhdWx0In19)](https://mermaid-js.github.io/mermaid-live-editor/#/edit/eyJjb2RlIjoiZ3JhcGggVEJcbnN1YmdyYXBoIEVtaXR0ZXIgU3Ryb3JlIEJ1ZmZlcnNcblx0YkJhc2ljPlBhcnRpY2xlRGF0YUJhc2ljXS0tPmlkMVsoR3B1QnVmZmVyKV1cblx0YlBoeXNpYz5QYXJ0aWNsZURhdGFQaHlzaWNdXG5lbmRcblxuc3ViZ3JhcGggM1xuICBQYXJ0aWNsZU1vZHVsZV9SZXNvbHZlUGh5c2ljLS0-QWRkLS4gQWRkIFBhcnRpY2xlRGF0YVBoeXNpYyAuLT5pZDEtLT5iUGh5c2ljXG5lbmRcblxuc3ViZ3JhcGggMlxuICBQYXJ0aWNsZU1vZHVsZV9HcmF2aXR5XG5lbmRcblxuc3ViZ3JhcGggMVxuICBQYXJ0aWNsZU1vZHVsZV9BdHRyYWN0b3JcbmVuZFxuXG5cdCIsIm1lcm1haWQiOnsidGhlbWUiOiJkZWZhdWx0In19)

### Modules initialization
Each modules will get a reference of each Buffer dependencies.
[![](https://mermaid.ink/img/eyJjb2RlIjoiZ3JhcGggVEJcbnN1YmdyYXBoIEVtaXR0ZXIgU3Ryb3JlIEJ1ZmZlcnNcblx0YkJhc2ljPlBhcnRpY2xlRGF0YUJhc2ljXS0tPmlkMVsoR3B1QnVmZmVyKV1cblx0YlBoeXNpYz5QYXJ0aWNsZURhdGFQaHlzaWNdLS0-aWQxXG5lbmRcblxuXG5zdWJncmFwaCAyXG4gIFBhcnRpY2xlTW9kdWxlX0dyYXZpdHlcbmVuZFxuXG5zdWJncmFwaCAxXG4gIFBhcnRpY2xlTW9kdWxlX0F0dHJhY3RvclxuZW5kXG5cbmlkMS0uIEdldCBQYXJ0aWNsZURhdGFQaHlzaWMgLi0-UGFydGljbGVNb2R1bGVfQXR0cmFjdG9yXG5pZDEtLiBHZXQgUGFydGljbGVEYXRhUGh5c2ljIC4tPlBhcnRpY2xlTW9kdWxlX0dyYXZpdHkiLCJtZXJtYWlkIjp7InRoZW1lIjoiZGVmYXVsdCJ9fQ)](https://mermaid-js.github.io/mermaid-live-editor/#/edit/eyJjb2RlIjoiZ3JhcGggVEJcbnN1YmdyYXBoIEVtaXR0ZXIgU3Ryb3JlIEJ1ZmZlcnNcblx0YkJhc2ljPlBhcnRpY2xlRGF0YUJhc2ljXS0tPmlkMVsoR3B1QnVmZmVyKV1cblx0YlBoeXNpYz5QYXJ0aWNsZURhdGFQaHlzaWNdLS0-aWQxXG5lbmRcblxuXG5zdWJncmFwaCAyXG4gIFBhcnRpY2xlTW9kdWxlX0dyYXZpdHlcbmVuZFxuXG5zdWJncmFwaCAxXG4gIFBhcnRpY2xlTW9kdWxlX0F0dHJhY3RvclxuZW5kXG5cbmlkMS0uIEdldCBQYXJ0aWNsZURhdGFQaHlzaWMgLi0-UGFydGljbGVNb2R1bGVfQXR0cmFjdG9yXG5pZDEtLiBHZXQgUGFydGljbGVEYXRhUGh5c2ljIC4tPlBhcnRpY2xlTW9kdWxlX0dyYXZpdHkiLCJtZXJtYWlkIjp7InRoZW1lIjoiZGVmYXVsdCJ9fQ)

### Obvious execution order

[![](https://mermaid.ink/img/eyJjb2RlIjoiZ3JhcGggVEJcblxuXG5QYXJ0aWNsZU1vZHVsZV9HcmF2aXR5LS0-UGFydGljbGVNb2R1bGVfQXR0cmFjdG9yLS0-UGFydGljbGVNb2R1bGVfUmVzb2x2ZVBoeXNpY1xuXG5cbnN1YmdyYXBoIDFcblBhcnRpY2xlTW9kdWxlX0dyYXZpdHktLT5ncHVDb21wdXRlR3Jhdml0eShbR1BVX0NvbXB1dGVfR3Jhdml0eV0pXG5lbmRcblxuc3ViZ3JhcGggMlxuUGFydGljbGVNb2R1bGVfQXR0cmFjdG9yLS0-Z3B1Q29tcHV0ZUF0dHJhY3RvcihbR1BVX0NvbXB1dGVfQXR0cmFjdG9yXSlcbmVuZFxuXHRcbnN1YmdyYXBoIDNcblBhcnRpY2xlTW9kdWxlX1Jlc29sdmVQaHlzaWMtLT5ncHVDb21wdXRlUmVzb2x2ZVBoeXNpYyhbR1BVX0NvbXB1dGVfUmVzb2x2ZVBoeXNpY10pXG5lbmRcbiIsIm1lcm1haWQiOnsidGhlbWUiOiJkZWZhdWx0In19)](https://mermaid-js.github.io/mermaid-live-editor/#/edit/eyJjb2RlIjoiZ3JhcGggVEJcblxuXG5QYXJ0aWNsZU1vZHVsZV9HcmF2aXR5LS0-UGFydGljbGVNb2R1bGVfQXR0cmFjdG9yLS0-UGFydGljbGVNb2R1bGVfUmVzb2x2ZVBoeXNpY1xuXG5cbnN1YmdyYXBoIDFcblBhcnRpY2xlTW9kdWxlX0dyYXZpdHktLT5ncHVDb21wdXRlR3Jhdml0eShbR1BVX0NvbXB1dGVfR3Jhdml0eV0pXG5lbmRcblxuc3ViZ3JhcGggMlxuUGFydGljbGVNb2R1bGVfQXR0cmFjdG9yLS0-Z3B1Q29tcHV0ZUF0dHJhY3RvcihbR1BVX0NvbXB1dGVfQXR0cmFjdG9yXSlcbmVuZFxuXHRcbnN1YmdyYXBoIDNcblBhcnRpY2xlTW9kdWxlX1Jlc29sdmVQaHlzaWMtLT5ncHVDb21wdXRlUmVzb2x2ZVBoeXNpYyhbR1BVX0NvbXB1dGVfUmVzb2x2ZVBoeXNpY10pXG5lbmRcbiIsIm1lcm1haWQiOnsidGhlbWUiOiJkZWZhdWx0In19)
