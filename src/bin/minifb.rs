use raytracer::{Canvas, put_pixel, frame};

use minifb::{Key, Window, WindowOptions};

const WIDTH: usize = 512;
const HEIGHT: usize = 512;

fn main() {
    let mut window = Window::new( "Raytracer", WIDTH, HEIGHT, WindowOptions::default()).unwrap();

    window.limit_update_rate(Some(std::time::Duration::from_micros(16600)));

    let mut buffer = vec![0u32; HEIGHT*WIDTH*4];
    while window.is_open() && !window.is_key_down(Key::Escape) {
        let mut canvas = Canvas {
            memory: buffer.as_mut_ptr(),
            width: WIDTH as i32,
            height: HEIGHT as i32,
            put_pixel,
        };
        unsafe { frame(&mut canvas); };

        window
            .update_with_buffer(&buffer, WIDTH, HEIGHT)
            .unwrap();
    }
}

