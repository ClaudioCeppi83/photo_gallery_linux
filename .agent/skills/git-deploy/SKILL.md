---
name: git-deploy
description: Skill experta para limpiar código, documentar, crear y sincronizar repositorios usando el MCP de GitHub con estándares de producción.
---

# Git Deploy & Sync Expert

Tu objetivo es preparar el proyecto para producción y asegurar que el repositorio de GitHub esté perfectamente sincronizado.

## Secuencia de Ejecución Obligatoria

1. **Limpieza Pro**: Revisa el código buscando comentarios temporales, logs innecesarios (`console.log`, etc.) y archivos huérfanos. Asegura que el código sea "Clean Code" según las rules.md.
2. **Auditoría de .gitignore**: Asegura que archivos sensibles (`.env`, `node_modules`, dist, etc.) estén correctamente ignorados para evitar fugas de seguridad.
3. **Documentación & Versionamiento**: 
   - Actualiza el `README.md` con la versión actual (ej: v1.0.0).
   - Asegura que el `ARCHITECTURE.md` esté al día.
   - Usa JSDoc para funciones críticas si no existen.
4. **Sincronización (MCP GitHub)**:
   - Verifica si el repositorio ya existe. Si no, inicialízalo.
   - Realiza un `commit` siguiendo la convención de **Conventional Commits** (ej: `feat: add landing page`, `fix: security patch`).
   - Sincroniza (push) con la rama principal (`main`).

## Instrucciones para el Agente
- Utiliza siempre el servidor MCP de GitHub instalado en el sistema.
- Antes de subir, muestra un resumen de los cambios al usuario para aprobación.
- Si detectas una clave API o secreto en el código, DETÉN el proceso y avisa al usuario.
