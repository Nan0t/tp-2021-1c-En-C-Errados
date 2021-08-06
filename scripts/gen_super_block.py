import sys

if __name__ == "__main__":
    if(len(sys.argv) < 2):
        print("Ingrese los siguientes argumentos: [MOUNT_POINT] [BLOCK_SIZE] [BLOCK_COUNT]")
    else:
        try:
            mount_point = sys.argv[1]
            block_size = int(sys.argv[2])
            block_count = int(sys.argv[3])
            bitmap = 0

            super_block = open(f"{mount_point}/SuperBloque.ims", "wb")
            
            super_block.write(block_size.to_bytes(4, "little"))
            super_block.write(block_count.to_bytes(4, "little"))
            super_block.write(bitmap.to_bytes(4, "little"))

            super_block.close()

        except ValueError as err:
            print(f"{format(err)}")