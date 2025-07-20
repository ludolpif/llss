use bevy::{prelude::*, render::view::RenderLayers};
use bevy_egui::{EguiGlobalSettings, PrimaryEguiContext};

use crate::{core_components::MainCamera, demo_scene::DemoScenePlugin};

mod core_components;
mod demo_scene;

fn main() {
    App::new()
    .add_plugins((DefaultPlugins, DemoScenePlugin))
    .add_systems(Startup, setup)
    //.add_systems(Update, (hello_world, greet_people))
    .run();
}

fn setup(
    mut commands: Commands,
    //mut egui_global_settings: ResMut<EguiGlobalSettings>,
) {
    //egui_global_settings.auto_create_primary_context = false;

    commands.spawn((
        Camera2d,
        Transform::IDENTITY,
        MainCamera
    ));

    /* egui camera
    commands.spawn((
        Camera2d,
        Name::new("Egui Camera"),
        PrimaryEguiContext,
        RenderLayers::none(),
        Camera {
            order: 1,
            ..default()
        },
    ));*/
}
