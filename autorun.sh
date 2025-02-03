#!/bin/bash

# Check if the ./bin directory exists
if [ -d "./bin" ]; then
  echo "Directory ./bin exists."
  
  # Check if the chessserver binary exists in ./bin
  if [ -f "./bin/chess-server" ]; then
    echo "Binary file chess-server exists in ./bin."
  else
    echo "Binary file chessserver does not exist in ./bin. Running ./autogen.sh..."
    if [ -f "./autogen.sh" ]; then
      ./autogen.sh
    else
      echo "File ./autogen.sh not found. Exiting."
      exit 1
    fi
  fi
else
  echo "Directory ./bin does not exist. Running ./autogen.sh..."
  if [ -f "./autogen.sh" ]; then
    ./autogen.sh
  else
    echo "File ./autogen.sh not found. Exiting."
    exit 1
  fi
fi

# Check if the djangoproj directory exists
if [ -d "./djangoproj" ]; then
  echo "Navigating to djangoproj directory..."
  cd ./djangoproj || exit
  
  # Check if manage.py exists in the djangoproj directory
  if [ -f "manage.py" ]; then
    echo "Starting Django server..."
    python3 manage.py runserver
  else
    echo "File manage.py not found in djangoproj. Exiting."
    exit 1
  fi
else
  echo "Directory djangoproj does not exist. Exiting."
fi
