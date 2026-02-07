# True Native Project Standards (No Web Wrappers)

## 1. Stack Tecnológico por Plataforma (Best-in-Class)
- **iOS & macOS (Apple)**: Lenguaje **Swift**. UI Framework: **SwiftUI**.
- **Android**: Lenguaje **Kotlin**. UI Framework: **Jetpack Compose**.
- **Windows**: Lenguaje **C#**. UI Framework: **WinUI 3** (Windows App SDK).
- **Linux**: Lenguaje **Rust** (por seguridad y modernidad). UI Framework: **GTK4** (via gtk-rs).

## 2. Identidad Visual: "Organic Native" (Estilo A + C)
Aunque el código sea diferente, la estética debe unificarse:
- **Filosofía**: Minimalismo cálido usando los componentes nativos de cada OS, pero estilizados.
- **Formas**: Bordes `CornerRadius` de 16dp/16pt (Apple/Android) o 12px (Windows).
- **Sombras**: Usar APIs de elevación nativa (`shadowRadius` en iOS, `elevation` en Android) muy suaves.
- **Tipografía**: San Francisco (Apple), Roboto/Inter (Android), Segoe UI Variable (Windows).

## 3. Seguridad & Datos (Firebase Native)
- **Backend**: Firebase usando los **SDKs Nativos** (no la API REST, ni JS SDK).
  - iOS: `Firebase/Auth`, `Firebase/Firestore`.
  - Android: `com.google.firebase:firebase-auth-ktx`.
- **Almacenamiento Seguro (BYOK)**:
  - iOS/Mac: **Keychain Services**.
  - Android: **EncryptedSharedPreferences** (MasterKey).
  - Windows: **Windows.Security.Credentials.PasswordVault**.
  - Linux: **Secret Service API** (libsecret).

## 4. Protocolo de Calidad
- **Arquitectura**: MVVM (Model-View-ViewModel) estricto en todas las plataformas para mantener lógica similar.
- **Git Sync**: Limpieza de build folders (`build/`, `DerivedData/`, `bin/`, `target/`) -> Tests Unitarios Nativos -> Commit Semántico.
