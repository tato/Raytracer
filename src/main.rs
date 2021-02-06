use pixels::{Pixels, SurfaceTexture};
use winit::dpi::LogicalSize;
use winit::event::{Event, VirtualKeyCode};
use winit::event_loop::{ControlFlow, EventLoop};
use winit::window::WindowBuilder;
use winit_input_helper::WinitInputHelper;

#[repr(C)]
#[derive(Debug, Clone, Copy)]
struct RGBA { r: u8, g: u8, b: u8, a: u8 }

type PutPixelFn = extern "C" fn(*mut Canvas, i32, i32, RGBA);

#[repr(C)]
struct Canvas {
    memory: *mut u8,
    width: i32,
    height: i32,
    put_pixel: PutPixelFn,
}
extern "C" { fn frame(canvas: *mut Canvas); }


extern "C" fn put_pixel(canvas: *mut Canvas, x: i32, y: i32, color: RGBA) {
    let canvas: &mut Canvas = unsafe { &mut (*canvas) };
    let x = canvas.width / 2 + x;
    let y = canvas.height / 2 - y;
    let i = (y * canvas.width + x) as usize;
    unsafe {
        canvas.memory.add(i*4).write(color.r);
        canvas.memory.add(i*4 + 1).write(color.g);
        canvas.memory.add(i*4 + 2).write(color.b);
        canvas.memory.add(i*4 + 3).write(color.a);
    }
}


fn main() {
    let width: i32 = 512;
    let height: i32 = 512;

    let event_loop = EventLoop::new();
    let mut input = WinitInputHelper::new();
    let window = {
        let size = LogicalSize::new(width as f64, height as f64);
        WindowBuilder::new()
            .with_title("Hello Pixels")
            .with_inner_size(size)
            .with_min_inner_size(size)
            .build(&event_loop)
            .unwrap()
    };

    let mut pixels = {
        let window_size = window.inner_size();
        let surface_texture = SurfaceTexture::new(window_size.width, window_size.height, &window);
        Pixels::new(width as u32, height as u32, surface_texture).unwrap()
    };

    event_loop.run(move |event, _, control_flow| {
        if let Event::RedrawRequested(_) = event {
            let buffer = pixels.get_frame();
            let mut canvas = Canvas { memory: buffer.as_mut_ptr(), width, height, put_pixel };
            unsafe { frame(&mut canvas); };

            if pixels.render().is_err() {
                *control_flow = ControlFlow::Exit;
                return;
            }
        }

        if input.update(&event) {
            if input.key_pressed(VirtualKeyCode::Escape) || input.quit() {
                *control_flow = ControlFlow::Exit;
                return;
            }
            if let Some(size) = input.window_resized() {
                pixels.resize(size.width, size.height);
            }

            // world.update();
            window.request_redraw();
        }
    });
}
