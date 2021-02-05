fn main() {
    cc::Build::new()
        .file("ray/ray.c")
        .include("ray")
        .compile("ray");
    println!("cargo:rustc-link-lib=ray")
}
