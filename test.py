def compare_binary_files(file1, file2):
    """Compares the contents of two binary files."""
    try:
        with open(file1, 'rb') as f1, open(file2, 'rb') as f2:
            while True:
                chunk1 = f1.read(4096)  # Read 4KB at a time
                chunk2 = f2.read(4096)
                if chunk1 != chunk2:
                    return False
                if not chunk1:  # End of both files
                    break
        return True
    except Exception as e:
        print(f"Error comparing files: {e}")
        return False

# Example usage
file1 = "data/test.bin"
file2 = "output.bin"

if compare_binary_files(file1, file2):
    print("The files are identical.")
else:
    print("The files are different.")


def output_first_and_last_100_bytes(file):
    """Prints the first and last 100 bytes of a binary file byte-by-byte."""
    try:
        with open(file, 'rb') as f:
            # Read the first 100 bytes
            first_100 = f.read(5000)

            # Move to the end and read the last 100 bytes
            f.seek(-5000, 2)  # Seek from the end of the file
            last_100 = f.read(5000)

            print(f"\nFile: {file}")
            print("First 100 bytes:")
            print(' '.join(f'{byte:02X}' for byte in first_100))

            print("\nLast 100 bytes:")
            print(' '.join(f'{byte:02X}' for byte in last_100))

    except Exception as e:
        print(f"Error reading file {file}: {e}")

# Example usage



# Example usage
file1 = "input.bin"
# file2 = "file2.bin"

output_first_and_last_100_bytes(file1)
# output_first_and_last_100_bytes(file2)
