plugins {
    kotlin("jvm") version "2.4.20"
}

group = "org.example"
version = "1.0-SNAPSHOT"

repositories {
    mavenCentral()
}

val lwjglVersion = "3.4.3"
val lwjglNatives = "natives-windows"

dependencies {
    implementation(platform("org.lwjgl:lwjgl-bom:$lwjglVersion"))

    implementation("org.lwjgl:lwjgl")
    implementation("org.lwjgl:lwjgl-sdl")
    implementation("org.lwjgl:lwjgl::$lwjglNatives")
    implementation("org.lwjgl:lwjgl-sdl::$lwjglNatives")
    
    // Tests
    testImplementation(kotlin("test"))
}

kotlin {
    jvmToolchain(25)
}

tasks.test {
    useJUnitPlatform()
}
