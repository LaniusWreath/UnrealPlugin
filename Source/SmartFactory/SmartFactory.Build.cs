using UnrealBuildTool;

public class SmartFactory : ModuleRules
{
    public SmartFactory(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
                "SmartFactory/Public"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "SmartFactory/Private"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "HTTP",
                "Json",
                "JsonUtilities",
                "UMG",
                "Sockets",
                "Networking",
                "WebSockets",
                "ProceduralMeshComponent"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // No dynamically loaded modules for now
            }
        );
    }
}
