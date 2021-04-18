fn main() {
    // std::env::set_var("CC", "clang");
    cc::Build::new()
        .file("ray/ray.c")
        .include("ray")
        .compile("ray");
    println!("cargo:rustc-link-lib=ray")
}
