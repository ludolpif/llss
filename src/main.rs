use bevy::prelude::*;

use crate::core_components::*;

mod core_components;
mod demo_scene;
mod ui_main;

fn main() {
    App::new()
    .add_plugins((
        DefaultPlugins,
        bevy_framepace::FramepacePlugin,
        demo_scene::DemoScenePlugin,
        ui_main::UIMainPlugin,
    ))
    .add_systems(Startup, setup)
    //.add_systems(Update, (hello_world, greet_people))
    .run();
}

fn setup(
    mut commands: Commands,
) {
    commands.spawn((
        Camera2d,
        Transform::IDENTITY,
        MainCamera
    ));
}
