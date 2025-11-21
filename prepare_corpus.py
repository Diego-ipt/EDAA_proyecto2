import os
import glob

def create_corpus(output_file, map_file, filenames_list_file, file_list):
    with open(output_file, 'wb') as outfile, \
         open(map_file, 'w') as mapfile, \
         open(filenames_list_file, 'w') as namesfile:
        
        for i, filepath in enumerate(file_list):
            doc_id = i + 1
            filename = os.path.basename(filepath)
            
            # Write mapping for human reference
            mapfile.write(f"Documento {doc_id}: {filename}\n")
            # Write filename for C++ program
            namesfile.write(f"{filename}\n")
            
            print(f"Processing Document {doc_id}: {filename}")
            
            # Read and write content
            with open(filepath, 'rb') as infile:
                outfile.write(infile.read())
            
            # Add separator if not the last file
            if i < len(file_list) - 1:
                outfile.write(b'\x02')

def main():
    # Define paths
    base_dir = os.path.dirname(os.path.abspath(__file__))
    datasets_dir = os.path.join(base_dir, 'datasets')
    
    # Collect files from all 3 directories
    files = []
    for subdir in ['dblp', 'proteins', 'sources']:
        path = os.path.join(datasets_dir, subdir, '*')
        # Sort to ensure deterministic order
        subdir_files = sorted(glob.glob(path))
        files.extend(subdir_files)
    
    output_corpus = os.path.join(base_dir, 'corpus.txt')
    output_map = os.path.join(base_dir, 'corpus_map.txt')
    output_names = os.path.join(base_dir, 'filenames.txt')
    
    print(f"Creating corpus from {len(files)} files...")
    create_corpus(output_corpus, output_map, output_names, files)
    print(f"Corpus created at: {output_corpus}")
    print(f"Map created at: {output_map}")
    print(f"Filenames list created at: {output_names}")

if __name__ == "__main__":
    main()
