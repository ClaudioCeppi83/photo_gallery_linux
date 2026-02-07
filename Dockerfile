FROM ubuntu:24.04

# Evitar prompts interactivos durante la instalación
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependencias esenciales de compilación y GTK 3
RUN apt-get update && apt-get install -y \
    build-essential \
    libgtk-3-dev \
    pkg-config \
    adwaita-icon-theme-full \
    && rm -rf /var/lib/apt/lists/*

# Crear usuario no root por seguridad
RUN useradd -m developer
USER developer
WORKDIR /home/developer/app

# Copiar el código fuente
COPY --chown=developer:developer . .

# Compilar la aplicación
RUN make

# Configuración para permitir la conexión con el servidor X11/Wayland del host
ENV DISPLAY=:0
ENV XDG_RUNTIME_DIR=/tmp/runtime-developer

# Comando por defecto
CMD ["./photo_gallery"]
