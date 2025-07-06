fn main() {
    println!("Hello, world!");
    // Print all environment variables.
    for (key, value) in std::env::vars() {
        println!("{key}: {value}");
    }
}
