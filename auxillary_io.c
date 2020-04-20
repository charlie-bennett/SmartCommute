
void ADC_init(void)
{
  ADMUX |= (1 << REFS0 ); // Set reference to AVCC
  ADMUX &= ~(1 << REFS1 );
  ADMUX |= (1 << ADLAR ); // Left adjust the output
  ADCSRA |= (7 << ADPS0 ); // Set the prescalar to 128
  ADCSRA |= (1 << ADEN ); // Enable the ADC
  return; 
}
//goes in while loop 
double ADC_poll(unit8_t channel, char iters)
{
  ADMUX |= (channel << MUX0 ); // Set the channel 
  double runsum = 0; 
  
  for (int i = 0; i<iters; i++)
  {
    ADCSRA |= (1 << ADSC ); // Start a conversion
    while ( ADCSRA & (1 << ADSC )); // wait for conversion complete
    runsum+=(5<<ADCH);
  }
  double runsum/iters; 
  
  /*
  Do we need to account for left justify or does prescalar value do this for us? 
  */
}

void push_back_FIFO(double val, double* buffer, char BUFFER_SIZE, char& current_loc)
{
  buffer[(current_loc%BUFFER_SIZE)] = val; 
  current_loc++; 
  return; 
}

double get_moving_average(double* buffer, char BUFFER_SIZE)
{
  double runsum; 
  for (int i = 0; i<BUFFER_SIZE: i++)
  {
    runsum+=buffer[i]; 
  }
  return runsum/(double BUFFER_SIZE); 
}




