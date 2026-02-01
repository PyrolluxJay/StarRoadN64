use std::fs::{self, File};
use std::io::{self, BufReader};
use png;

fn load_image(file: &File) -> Result<Vec<u8>, png::DecodingError>
{
    let decoder = png::Decoder::new(BufReader::new(file));
    let mut reader = decoder.read_info()?;
    let Some(buf_size) = reader.output_buffer_size() else {
        return Err(png::DecodingError::LimitsExceeded);
    };
    let mut buf = vec![0; buf_size];
    let info = reader.next_frame(&mut buf)?;
    buf.resize(info.buffer_size(), 0);
    Ok(buf)
}

fn is_transparent(image: &[u8]) -> bool
{
    image.iter()
         .skip(3)
         .step_by(4)
         .all(|x| *x == 0)
}

fn main() -> io::Result<()> {
    for level_entry in fs::read_dir("../../levels")? {
        let level_path = level_entry?.path();

        let custom_c_path = {
            let mut path = level_path.clone();
            path.push("custom_c");
            path
        };
        if !custom_c_path.exists() {
            continue;
        }
        
        let custom_textures_path = {
            let mut path = level_path.clone();
            path.push("custom_textures");
            path
        };
        let Ok(custom_textures_entries) = fs::read_dir(&custom_textures_path) else {
            println!("No texture path exists for {custom_textures_path:#?}");
            continue
        };

        for texture_entry in custom_textures_entries {
            let texture_path = texture_entry?.path();
            let Ok(texture_file) = File::open(&texture_path) else {
                println!("Failed to open texture file {texture_path:#?}");
                continue
            };

            let Ok(image) = load_image(&texture_file) else {
                println!("Failed to decode {texture_path:#?}");
                continue;
            };

            let transparent = is_transparent(&image);
            if transparent {
                println!("Transparent texture detected {texture_path:#?}");
            }
        }
    }

    Ok(())
}
