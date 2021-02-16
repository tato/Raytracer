#[repr(C)]
#[derive(Debug, Clone, Copy)]
pub struct RGBA {
    r: u8,
    g: u8,
    b: u8,
    a: u8,
}

pub type PutPixelFn = extern "C" fn(*mut Canvas, i32, i32, RGBA);

#[repr(C)]
pub struct Canvas {
    pub memory: *mut u8,
    pub width: i32,
    pub height: i32,
    pub put_pixel: PutPixelFn,
}
extern "C" {
    pub fn frame(canvas: *mut Canvas);
}

pub extern "C" fn put_pixel(canvas: *mut Canvas, x: i32, y: i32, color: RGBA) {
    let canvas: &mut Canvas = unsafe { &mut (*canvas) };
    let x = canvas.width / 2 + x;
    let y = canvas.height / 2 - y;
    let i = (y * canvas.width + x) as usize;
    unsafe {
        canvas.memory.add(i * 4).write(color.r);
        canvas.memory.add(i * 4 + 1).write(color.g);
        canvas.memory.add(i * 4 + 2).write(color.b);
        canvas.memory.add(i * 4 + 3).write(color.a);
    }
}
