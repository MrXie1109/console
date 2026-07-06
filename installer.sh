#!/bin/sh
# Installation script for console library
# Goal: Install https://github.com/MrXie1109/console to /usr/local/include

set -e

# --- Terminal color definitions ---
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# --- Helper functions ---
info() {
    printf "${BLUE}[INFO]${NC} %s\n" "$1"
}

success() {
    printf "${GREEN}[SUCCESS]${NC} %s\n" "$1"
}

warning() {
    printf "${YELLOW}[WARNING]${NC} %s\n" "$1"
}

error() {
    printf "${RED}[ERROR]${NC} %s\n" "$1" >&2
}

step() {
    printf "\n${CYAN}${BOLD}>>>${NC} ${CYAN}%s${NC}\n" "$1"
}

# --- 1. Check Root Privileges ---
step "Checking root privileges"
if [ "$(id -u)" -ne 0 ]; then
    error "This installation script requires root privileges. Please run with sudo."
    exit 1
fi
success "Root privileges confirmed"

# --- 2. Check Git ---
step "Checking system dependencies"
if ! command -v git >/dev/null 2>&1; then
    error "git command not found. Please install git first."
    exit 1
fi
success "git found: $(command -v git)"

# --- 3. Ask for Clone Protocol ---
step "Select clone protocol"
printf "${BOLD}1)${NC} HTTPS (recommended, usually no extra configuration needed)\n"
printf "${BOLD}2)${NC} SSH (requires that you have already configured SSH keys)\n"
printf "Enter 1 or 2 [default: 1]: "
read -r protocol_choice

if [ "$protocol_choice" = "2" ]; then
    REPO_URL="git@github.com:MrXie1109/console.git"
    info "SSH protocol selected"
else
    REPO_URL="https://github.com/MrXie1109/console.git"
    info "HTTPS protocol selected"
fi

# --- 4. Prepare Temporary Directory and Clone ---
step "Preparing clone environment"
TEMP_DIR=$(mktemp -d)
info "Temporary directory: $TEMP_DIR"
if [ "$protocol_choice" = "2" ] && [ -n "$SUDO_USER" ]; then
    chown "$SUDO_USER":"$SUDO_USER" "$TEMP_DIR"
    info "Changed ownership of $TEMP_DIR to $SUDO_USER for SSH clone"
fi
info "Cloning repository from $REPO_URL"

# If using SSH under sudo, clone as the original user to preserve SSH keys
if [ "$protocol_choice" = "2" ] && [ -n "$SUDO_USER" ]; then
    info "Running in sudo environment, cloning as user: $SUDO_USER"
    if ! sudo -u "$SUDO_USER" git clone --depth 1 "$REPO_URL" "$TEMP_DIR" 2>&1; then
        error "git clone failed. Please verify your SSH key configuration."
        error "Test your SSH connection with: sudo -u $SUDO_USER ssh -T git@github.com"
        rm -rf "$TEMP_DIR"
        exit 1
    fi
elif [ "$protocol_choice" = "2" ] && [ -z "$SUDO_USER" ]; then
    # Running as root directly (not via sudo), SSH may still work if root has keys
    if ! git clone --depth 1 "$REPO_URL" "$TEMP_DIR" 2>&1; then
        error "git clone failed. Please verify that root has SSH keys configured."
        error "Test with: ssh -T git@github.com"
        rm -rf "$TEMP_DIR"
        exit 1
    fi
else
    # HTTPS clone
    if ! git clone --depth 1 "$REPO_URL" "$TEMP_DIR" 2>&1; then
        error "git clone failed. Please check your network connection."
        rm -rf "$TEMP_DIR"
        exit 1
    fi
fi
success "Repository cloned successfully"

# --- 5. Install Header Files ---
step "Installing header files"
SOURCE_DIR="$TEMP_DIR/console"
if [ ! -d "$SOURCE_DIR" ]; then
    error "Cannot find 'console' subdirectory in cloned repository."
    error "Please examine the contents of: $TEMP_DIR"
    rm -rf "$TEMP_DIR"
    exit 1
fi

TARGET_DIR="/usr/local/include/console"
BACKUP_DIR=""  # Initialize variable to track if backup was created

# Safely back up existing installation
if [ -d "$TARGET_DIR" ]; then
    BACKUP_DIR="${TARGET_DIR}.backup.$(date +%Y%m%d%H%M%S)"
    warning "Target directory $TARGET_DIR already exists"
    info "Creating backup: $BACKUP_DIR"
    mv "$TARGET_DIR" "$BACKUP_DIR"
fi

info "Copying to $TARGET_DIR"
cp -r "$SOURCE_DIR" "$TARGET_DIR"
success "Header files installed"

# --- 6. Cleanup and Finish ---
step "Cleaning up"
rm -rf "$TEMP_DIR"
info "Removed temporary directory: $TEMP_DIR"

echo ""
success "Installation completed successfully"
echo ""
printf "${BOLD}Installation summary:${NC}\n"
printf "  Location: ${CYAN}%s${NC}\n" "$TARGET_DIR"
printf "  Usage:    ${CYAN}#include <console/all.h>${NC}\n"
printf "  Type:     ${CYAN}Header-only library${NC}\n"

# --- 7. Backup cleanup reminder ---
if [ -n "$BACKUP_DIR" ] && [ -d "$BACKUP_DIR" ]; then
    echo ""
    printf "${YELLOW}${BOLD}>>> Backup reminder:${NC}\n"
    printf "  Old version backed up to: ${CYAN}%s${NC}\n" "$BACKUP_DIR"
    printf "  You can remove it with: ${CYAN}sudo rm -rf '%s'${NC}\n" "$BACKUP_DIR"
    printf "  ${YELLOW}Note: Keep it if you need to rollback${NC}\n"
fi

echo ""
