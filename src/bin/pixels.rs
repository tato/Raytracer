use pixels::{Pixels, SurfaceTexture};
use winit::dpi::LogicalSize;
use winit::event::{Event, VirtualKeyCode};
use winit::event_loop::{ControlFlow, EventLoop};
use winit::window::WindowBuilder;
use winit_input_helper::WinitInputHelper;
use raytracer::{Canvas, put_pixel, frame};

fn main() {
    let event_loop = EventLoop::new();
    let mut input = WinitInputHelper::new();
    let window = {
        let size = LogicalSize::new(512.0, 512.0);
        WindowBuilder::new()
            .with_resizable(false)
            .with_title("Raytracer")
            .with_inner_size(size)
            .with_min_inner_size(size)
            .build(&event_loop)
            .unwrap()
    };

    let mut pixels = {
        let window_size = window.inner_size();
        let surface_texture = SurfaceTexture::new(window_size.width, window_size.height, &window);
        Pixels::new(
            window_size.width as u32,
            window_size.height as u32,
            surface_texture,
        )
        .unwrap()
    };

    event_loop.run(move |event, _, control_flow| {
        if let Event::RedrawRequested(_) = event {
            let window_size = window.inner_size();
            let buffer = pixels.get_frame();
            let mut canvas = Canvas {
                memory: buffer.as_mut_ptr(),
                width: window_size.width as i32,
                height: window_size.height as i32,
                put_pixel,
            };
            unsafe {
                frame(&mut canvas);
            };

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
