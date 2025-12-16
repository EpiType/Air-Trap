// Colorblind shader for SFML
// Compatible with SFML 3.0 shader system

uniform sampler2D texture;
uniform int mode; // 0=None, 1=Protanopia, 2=Deuteranopia, 3=Tritanopia

void main() {
    vec4 color = texture2D(texture, gl_TexCoord[0].xy);
    
    if (mode == 1) {
        // Protanopia (Red-blind)
        float r = 0.567 * color.r + 0.433 * color.g;
        float g = 0.558 * color.r + 0.442 * color.g;
        float b = 0.242 * color.g + 0.758 * color.b;
        gl_FragColor = vec4(r, g, b, color.a);
    } else if (mode == 2) {
        // Deuteranopia (Green-blind)
        float r = 0.625 * color.r + 0.375 * color.g;
        float g = 0.7 * color.r + 0.3 * color.g;
        float b = 0.3 * color.g + 0.7 * color.b;
        gl_FragColor = vec4(r, g, b, color.a);
    } else if (mode == 3) {
        // Tritanopia (Blue-blind)
        float r = 0.95 * color.r + 0.05 * color.g;
        float g = 0.433 * color.g + 0.567 * color.b;
        float b = 0.475 * color.g + 0.525 * color.b;
        gl_FragColor = vec4(r, g, b, color.a);
    } else {
        // Mode normal
        gl_FragColor = color;
    }
}