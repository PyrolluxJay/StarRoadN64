import argparse
import sys
from typing import Optional

import xml.etree.ElementTree as ET

class ColladaParser:
    def __init__(self, dae_file_path: str):
        self.file_path = dae_file_path
        self.tree: Optional[ET.ElementTree] = None
        self.root: Optional[ET.Element] = None
    
    def parse(self) -> ET.Element:
        """Parse the COLLADA DAE file and return the root element."""
        try:
            self.tree = ET.parse(self.file_path)
            self.root = self.tree.getroot()
            return self.root
        except ET.ParseError as e:
            raise ValueError(f"Failed to parse XML file: {e}")
        except FileNotFoundError:
            raise FileNotFoundError(f"File not found: {self.file_path}")
    
    def get_tree(self) -> ET.ElementTree:
        """Get the ElementTree object for modification."""
        if self.tree is None:
            raise ValueError("File not parsed yet. Call parse() first.")
        return self.tree
    
    def serialize(self, output_path: Optional[str] = None, encoding: str = 'utf-8') -> str:
        """Serialize the XML tree back to DAE format."""
        if self.tree is None:
            raise ValueError("No tree to serialize. Call parse() first.")
        
        # Use original file path if no output path specified
        if output_path is None:
            output_path = self.file_path
        
        # Write to file
        self.tree.write(output_path, encoding=encoding, xml_declaration=True)
        
        return output_path

class Patcher:
    def __init__(self):
        self._technique_count = 0
        self._texcoord_count = 0

    def patch(self, node):
        lbracket = node.tag.rfind('}')
        tag = node.tag[lbracket + 1:]
        if tag == 'technique':
            node.attrib['sid'] = f"technique_{self._technique_count}"
            self._technique_count += 1
        if tag == 'texture':
            node.attrib['texcoord'] = f"texcoord{self._texcoord_count}"
            self._texcoord_count += 1

def mangle_name(name, suffix):
    ldot = name.rfind('.')
    name_body = name if ldot == -1 else name[:ldot]
    ext = '' if ldot == -1 else name[ldot:]
    return f"{name_body}{suffix}{ext}"

def main():
    input_file = sys.argv[1]

    # Create parser instance
    collada = ColladaParser(input_file)
    
    # Parse the file
    root = collada.parse()
    print(f"Successfully parsed {input_file}")
    print(f"Root element: {root.tag}")

    patcher = Patcher()

    nodes_to_check = [ (root, 0), ]
    while nodes_to_check:
        node, depth = nodes_to_check.pop()

        print(f"{'\t' * depth}Node: {node.tag}")
        patcher.patch(node)
        for child in node:
            nodes_to_check.append((child, depth + 1))

    # Serialize back to file
    output_path = mangle_name(input_file, "_test")
    collada.serialize(output_path)
    print(f"Serialized to {output_path}")

if __name__ == "__main__":
    main()