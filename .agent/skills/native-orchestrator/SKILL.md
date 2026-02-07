---
name: native-orchestrator
description: Organiza un proyecto Multi-Nativo (Monorepo) separando carpetas por plataforma.
---

# Native Project Orchestrator

Estructura el proyecto para mantener orden entre plataformas incompatibles.

## Estructura de Carpetas Obligatoria
- `/ios`: Proyecto Xcode (Swift).
- `/android`: Proyecto Gradle (Kotlin).
- `/windows`: Solución Visual Studio (C#).
- `/design`: Assets compartidos (Iconos, SVGs, Fuentes).
- `/docs`: Documentación unificada.

## Flujo de Trabajo
1. Si el usuario pide "Crea la pantalla de Login", debes generar el código para iOS (@apple-native) Y para Android (@android-native) simultáneamente (o preguntar cual priorizar).
2. Mantén la consistencia: Si el botón es azul en iOS, debe ser el mismo azul hexadecimal en Android.
