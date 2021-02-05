use pixels::{Pixels, SurfaceTexture};
use winit::dpi::LogicalSize;
use winit::event::{Event, VirtualKeyCode};
use winit::event_loop::{ControlFlow, EventLoop};
use winit::window::WindowBuilder;
use winit_input_helper::WinitInputHelper;


#[repr(C)]
struct Canvas {
    memory: *mut u8,
    width: u32,
    height: u32,
}
extern "C" { fn fill_canvas(canvas: *mut Canvas); }


fn main() {
    let width: u32 = 512;
    let height: u32 = 512;

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
        Pixels::new(width, height, surface_texture).unwrap()
    };

    event_loop.run(move |event, _, control_flow| {
        if let Event::RedrawRequested(_) = event {
            let frame = pixels.get_frame();
            let mut canvas = Canvas { memory: frame.as_mut_ptr(), width, height };
            unsafe { fill_canvas(&mut canvas); };

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
