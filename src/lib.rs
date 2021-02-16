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
    pub memory: *mut u32,
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
    let v: u32 = (color.r as u32) << 16
        | (color.g as u32) << 8 
        | (color.b as u32) << 0;
    unsafe { canvas.memory.add(i).write(v); }
}
