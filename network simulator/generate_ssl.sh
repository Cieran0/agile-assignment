#!/bin/bash

# Set file names for private key, certificate, and configuration
PRIVATE_KEY="server.key"
CERTIFICATE="server.crt"
CONFIG_FILE="openssl.cnf"

# Create OpenSSL configuration file with SAN
cat > $CONFIG_FILE <<EOL
[ req ]
default_bits        = 2048
default_keyfile     = $PRIVATE_KEY
distinguished_name  = req_distinguished_name
x509_extensions     = v3_ca

[ req_distinguished_name ]
C  = US
ST = California
L  = SanFrancisco
O  = ExampleCompany
OU = IT
CN = 127.0.0.1

[ v3_ca ]
subjectAltName = @alt_names

[ alt_names ]
DNS.1 = localhost
IP.1 = 127.0.0.1
EOL

# Generate an unencrypted 2048-bit RSA private key
echo "Generating private key..."
openssl genpkey -algorithm RSA -out $PRIVATE_KEY -pkeyopt rsa_keygen_bits:2048

# Generate a self-signed certificate using the private key and the custom configuration file
echo "Generating self-signed certificate..."
openssl req -new -x509 -key $PRIVATE_KEY -out $CERTIFICATE -days 365 -config $CONFIG_FILE

# Display the generated files
echo "SSL Private Key: $PRIVATE_KEY"
echo "SSL Certificate: $CERTIFICATE"

# Clean up by removing the configuration file
rm -f $CONFIG_FILE

# Confirm the certificate creation
echo "Certificate and key have been successfully generated."
