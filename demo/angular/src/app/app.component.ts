import { Component } from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.scss']
})
export class AppComponent {
  title = 'porcupine-angular-demo-porcupine';
  show = true;

  toggle(): void {
    this.show = !this.show;
  }
}
