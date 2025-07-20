use bevy::prelude::*;

pub struct DemoScenePlugin;

impl Plugin for DemoScenePlugin {
    fn build(&self, app: &mut App) {
        app.add_systems(Startup, setup);
    }
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<ColorMaterial>>,
) {
    let color = Color::hsl(90.0, 0.95, 0.7);
    let rect = meshes.add(Rectangle::new(120.0, 90.0));

    commands.spawn((
            Mesh2d(rect),
            MeshMaterial2d(materials.add(color)),
            Transform::IDENTITY,
            Name::new("Demo Scene")
    ));
}
