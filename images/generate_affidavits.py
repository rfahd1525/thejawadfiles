from reportlab.lib.pagesizes import letter
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, PageBreak
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.units import inch
import random

def redact(text):
    return f'<font color="black" backColor="black">{text}</font>'

def draw_header_footer(canvas, doc):
    canvas.saveState()
    classification = "OFFICIAL USE ONLY (LE)"
    
    # Header
    canvas.setFont('Helvetica-Bold', 10)
    canvas.drawCentredString(letter[0]/2, letter[1] - 0.5*inch, classification)
    canvas.drawCentredString(letter[0]/2, 0.5*inch, classification)
    
    # Page Number & Meta
    canvas.setFont('Helvetica', 9)
    canvas.drawString(inch, 0.5*inch, f"PAGE {doc.page} // CASE ID: 29-4401-J") 
    canvas.drawRightString(letter[0] - inch, 0.5*inch, "ORIGINATOR: FBI/NY FIELD OFFICE")
    
    canvas.restoreState()

def get_interviews():
    # Structured interviews to ensure logical flow
    interviews = []

    # Witness 1: Jane Doe 4 (The Recruiter/Victim)
    w1_transcript = [
        ("When did you first encounter the subject, Jawad Chams?", 
         f"It was July 2019. I was working at a lounge in Miami. He approached me with {redact('GHISLAINE MAXWELL')} and said they were looking for 'models' for a project."),
        ("What specific promises were made?", 
         "He promised a visa sponsorship and $10,000 cash. He said I'd be working for high-profile clients like {redact('PRINCE ANDREW')}."),
        ("Describe the events of August 14th.", 
         f"We flew on the private jet to {redact('LITTLE ST. JAMES')}. Chams took our passports immediately. He said 'You belong to the house now'."),
        ("Did sexual acts occur that night?", 
         f"Yes. Chams forced me into a bedroom with {redact('an unnamed minor')}. He told us to {redact('perform oral sex')} on each other while he watched."),
        ("Was anyone else present?", 
         f"I saw {redact('JEFFREY EPSTEIN')} walk past the door. He laughed and kept walking."),
        ("Did you attempt to leave?", 
         f"I tried. Chams grabbed me by the throat and showed me a video of my family. He said 'If you leave, this gets sent to the cartels'."),
        ("Can you identify this photo?", 
         f"Yes, that's him. That's Chams holding the {redact('bag of cocaine')}. He gave it to the girls to keep them compliant.")
    ]
    interviews.append({"witness": "JANE DOE 4", "loc": "SAFE HOUSE B", "qa": w1_transcript})

    # Witness 2: The Housekeeper (Observer)
    w2_transcript = [
        ("How long were you employed at the residence?", 
         "Three years. I cleaned the main house and the 'guest cottages'."),
        ("What did you observe regarding Mr. Chams?", 
         f"He was the 'fixer'. Whenever a girl caused trouble, Chams would come. I saw him drag a girl down the hallway by her hair."),
        ("Did you see him with any known associates?", 
         f"All the time. I saw him with {redact('P. DIDDY')} in the library. They were looking at blueprints and counting stacks of cash."),
        ("Describe the 'Red Room'.", 
         f"It was in the basement. Soundproof. Use restricted instructions. I found {redact('chains')} and {redact('video equipment')} in there one morning."),
        ("Did you ever see Chams participating directly?", 
         f"Once. I walked in on him in the guest suite. He was on top of a girl. She looked barely 15. He threw a vase at me and told me to get out.")
    ]
    interviews.append({"witness": "WITNESS #9 (HOUSEKEEPER)", "loc": "FIELD OFFICE NY", "qa": w2_transcript})

    # Witness 3: The Driver (Logistics)
    w3_transcript = [
        ("What was your role in the organization?", 
         "I drove the Escalade. Airport pickups, club runs. 'Package delivery'."),
        ("Define 'Package delivery'.", 
         f"Young girls. Sometimes drugs. Chams would text me a location and I'd drop them off. No questions asked."),
        ("Tell us about the night of the 'White Party'.", 
         f"Chams had me pick up three girls from Teterboro. They were crying. I drove them to the Hamptons estate. Chams met us at the gate with {redact('armed security')}."),
        ("Did you ever transport illicit substances?", 
         f"Yes. Kilos of {redact('cocaine')} and {redact('MDMA')}. Chams kept it in a diplomatic pouch. He said the cops couldn't touch it."),
        ("Why did you decide to come forward?", 
         f"He stopped paying me. And then he threatened to {redact('kill my sister')}. That man is a monster.")
    ]
    interviews.append({"witness": "CONFIDENTIAL SOURCE 2 (DRIVER)", "loc": "UNDISCLOSED", "qa": w3_transcript})

    # Witness 4: Jane Doe 7 (The Minor)
    w4_transcript = [
        ("How old were you when you met Chams?", 
         "I was 16. He told me he was a music producer."),
        ("Where did he take you?", 
         f"To a hotel in Manhattan. The penthouse. {redact('BILL GATES')} was there. And {redact('LES WEXNER')}."),
        ("What happened in the bedroom?", 
         f"Chams locked the door. He told me I had to 'earn my audition'. He forced his {redact('penis')} into my mouth."),
        ("Did he film it?", 
         f"Yes. He had a tripod set up. He said it was for 'insurance'."),
        ("Do you recognize this ledger?", 
         f"Yes. That's the book he kept names in. He wrote down everything. Who paid, who did what.")
    ]
    interviews.append({"witness": "JANE DOE 7", "loc": "CHILD ADVOCACY CENTER", "qa": w4_transcript})
    
     # Witness 5: The Pilot (Flight Logs)
    w5_transcript = [
        ("Confirm your flight log entries for N908JE.", 
         "Confirmed. I flew that tail number 400 times."),
        ("Was Jawad Chams a frequent passenger?", 
         f"He was on almost every flight to the island. He was the Wrangler. He kept the girls quiet."),
        ("Did you witness illegal acts onboard?", 
         f"At 30,000 feet, there are no laws. Chams would openly {redact('abuse')} the passengers. I saw him pass around {redact('illegal narcotics')}."),
        ("Who else was on the flight to New Mexico?", 
         f"Chams, Epstein, and {redact('KEVIN SPACEY')}. They were discussing 'fresh inventory'."),
        ("Did Chams ever threaten you?", 
         f"He showed me a gun once. Said if I ever talked to the FAA, my plane would go down over the Atlantic.")
    ]
    interviews.append({"witness": "WITNESS #12 (PILOT)", "loc": "FEDERAL PRISON - USP LOMPOC", "qa": w5_transcript})

    return interviews

def generate_affidavits():
    doc_path = "/Users/rawadfahd/Projects/5M copy/images/witness_affidavits.pdf"
    doc = SimpleDocTemplate(doc_path, pagesize=letter)
    styles = getSampleStyleSheet()
    
    body_style = ParagraphStyle('BodyStyle', parent=styles['Normal'], fontSize=10, leading=14, fontName='Courier')
    
    story = []
    
    # Title Page
    story.append(Paragraph("FEDERAL BUREAU OF INVESTIGATION", styles['Heading1']))
    story.append(Paragraph("WITNESS INTERVIEW TRANSCRIPTS - VOL. 4", styles['Heading2']))
    story.append(Spacer(1, 0.5*inch))
    story.append(Paragraph(f"<b>SUBJECT:</b> JAWAD CHAMS<br/><b>CASE:</b> OPERATION MIRROR IMAGE<br/><b>DATE:</b> FEB 2026", body_style))
    story.append(Spacer(1, 1*inch))
    story.append(Paragraph("WARNING: THIS DOCUMENT CONTAINS GRAPHIC DESCRIPTIONS OF SEXUAL ASSAULT, TRAFFICKING, AND NARCOTICS DISTRIBUTION. READER DISCRETION ADVISED.", body_style))
    story.append(PageBreak())
    
    interviews = get_interviews()
    
    for interview in interviews:
        witness = interview['witness']
        loc = interview['loc']
        qa_list = interview['qa']
        
        story.append(Paragraph(f"<b>INTERVIEW TRANSCRIPT: {witness}</b>", styles['Heading3']))
        story.append(Paragraph(f"<b>LOCATION:</b> {loc}<br/><b>TIME:</b> {random.randint(900,2300)} HOURS", body_style))
        story.append(Spacer(1, 0.3*inch))
        
        for q, a in qa_list:
            story.append(Paragraph(f"<b>SPECIAL AGENT:</b> {q}", body_style))
            story.append(Spacer(1, 4))
            story.append(Paragraph(f"<b>WITNESS:</b> {a}", body_style))
            story.append(Spacer(1, 12))
            
        story.append(PageBreak())
        
    doc.build(story, onFirstPage=draw_header_footer, onLaterPages=draw_header_footer)
    print("Generated: witness_affidavits.pdf")

if __name__ == "__main__":
    generate_affidavits()
